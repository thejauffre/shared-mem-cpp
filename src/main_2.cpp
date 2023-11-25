#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <vector>
#include <opencv2/opencv.hpp>

namespace bip = boost::interprocess;

int main()
{
    std::cout << "Hello Main 2" << std::endl;

    // Shared memory parameters
    // Calculate frame size in bytes
    const int numChannels = 3; // Assuming 3 channels for a typical color image (BGR)
    const int bitDepth = 8;    // Assuming 8 bits per channel
    const std::size_t frameSize = static_cast<std::size_t>(640 * 480 * numChannels * bitDepth / 8);
    const std::size_t sharedMemorySize = frameSize * 2; // Double buffer
    // Create shared memory
    bip::shared_memory_object sharedMemory(bip::open_or_create, "video_shared_memory", bip::read_write);
    sharedMemory.truncate(sharedMemorySize);

    // Map the shared memory to a region
    bip::mapped_region region(sharedMemory, bip::read_write);

    // Create semaphore for synchronization
    bip::named_semaphore semaphore(bip::open_or_create, "video_semaphore", 0);

    cv::Mat receivedFrame;

    // Loop to continuously receive frames from the child process
    while (true)
    {
        // Wait for the semaphore (indicating a new frame is available)
        semaphore.wait();

        // Deserialize the frame from shared memory
        std::vector<uchar> receivedData(static_cast<uchar *>(region.get_address()), static_cast<uchar *>(region.get_address()) + frameSize);
        receivedFrame = cv::imdecode(receivedData, cv::IMREAD_UNCHANGED);

        // Display the received frame (you might want to modify this based on your needs)
        cv::imshow("Received Frame", receivedFrame);
        cv::waitKey(1);
    }
}