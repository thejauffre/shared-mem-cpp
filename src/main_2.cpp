#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <vector>
#include <opencv2/opencv.hpp>
#include <chrono>

namespace bip = boost::interprocess;

int main()
{
    std::cout << "Hello Main 2" << std::endl;

    // Shared memory parameters
    // Calculate frame size in bytes
    const int numChannels = 3; // Assuming 3 channels for a typical color image (BGR)
    const int bitDepth = 8;    // Assuming 8 bits per channel
    const std::size_t frameSize = static_cast<std::size_t>(1280 * 720 * numChannels * bitDepth / 8);

    // Create shared memory
    bip::shared_memory_object sharedMemory(bip::open_or_create, "/image_rect_color", bip::read_write);
    const std::size_t sharedMemorySize = frameSize; // Double buffer
    sharedMemory.truncate(sharedMemorySize);
    bip::mapped_region region(sharedMemory, bip::read_write);
    void *sharedMemoryPtr = region.get_address();

    // Map the shared memory to a region
    std::cout << "sharedMemorySize: " << sharedMemorySize << " frameSize: " << frameSize << std::endl;
    if (sharedMemorySize < frameSize)
    {
        std::cerr << "Error: Shared memory size is less than frame size." << std::endl;
    }
    else if (sharedMemorySize > frameSize)
    {
        std::cerr << "Error: Shared memory size is greater than frame size." << std::endl;
    }

    cv::Mat receivedFrame;
    int frameCount = 1;
    double avgFramerate = 0.0;
    auto lastTime = std::chrono::high_resolution_clock::now();

    // Create a named mutex and condition variable
    bip::named_mutex mutex(bip::open_or_create, "SharedMemoryMutex");
    bip::named_condition cond(bip::open_or_create, "SharedMemoryCond");

    // Loop to continuously receive frames from the child process
    while (true)
    {
        bip::scoped_lock<bip::named_mutex> lock(mutex);
        cond.wait(lock);
        // Deserialize the frame from shared memory
        receivedFrame = cv::Mat(720, 1280, CV_8UC3, sharedMemoryPtr);
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        if (receivedFrame.empty())
            continue;

        double currentFramerate = 1.0 / elapsed.count();
        avgFramerate = ((avgFramerate * (frameCount - 1)) + currentFramerate) / frameCount;
        std::cout << "currentFramerate: " << currentFramerate << " avgFramerate: " << avgFramerate << std::endl;
        ++frameCount;
        cv::putText(receivedFrame,                              // target image
                    std::to_string(avgFramerate),               // text
                    cv::Point(10, receivedFrame.rows / 2 - 50), // top-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(118, 185, 0), // font color
                    2);

        // Display the received frame (you might want to modify this based on your needs)
        cv::imshow("Received Frame", receivedFrame);
        cv::waitKey(1);
    }
}