#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp> 
#include <vector>
#include <opencv2/opencv.hpp>

namespace bip = boost::interprocess;
using namespace cv;
// Create a VideoCapture and share its stream to the other application
int main()
{
    std::cout << "Hello Main 1" << std::endl;
    // OpenCV VideoCapture
    cv::VideoCapture capture(0);
    if (!capture.isOpened())
    {
        std::cerr << "Error: Unable to open video file." << std::endl;
        return 1;
    }

    // Calculate frame size in bytes
    const int numChannels = 3; // Assuming 3 channels for a typical color image (BGR)
    const int bitDepth = 8;    // Assuming 8 bits per channel
    const std::size_t frameSize = static_cast<std::size_t>(capture.get(CAP_PROP_FRAME_WIDTH) * capture.get(CAP_PROP_FRAME_HEIGHT) * numChannels * bitDepth / 8);
    const std::size_t sharedMemorySize = frameSize * 2; // Double buffer
    std::cout << "CV_CAP_PROP_FRAME_WIDTH " << capture.get(CAP_PROP_FRAME_WIDTH);
    std::cout << "\nCV_CAP_PROP_FRAME_HEIGHT " << capture.get(CAP_PROP_FRAME_HEIGHT);
    std::cout << "\nsharedMemorySize " << sharedMemorySize;
    std::cout << std::endl;

    // Create shared memory
    bip::shared_memory_object sharedMemory(bip::open_or_create, "video_shared_memory", bip::read_write);
    sharedMemory.truncate(sharedMemorySize);

    // Map the shared memory to a region
    bip::mapped_region region(sharedMemory, bip::read_write);

    // Create semaphore for synchronization
    bip::named_semaphore semaphore(bip::open_or_create, "video_semaphore", 0);

    cv::Mat frame(cv::Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT)), CV_8UC3);

    // Loop to continuously read frames from the video and share them
    while (true)
    {
        // Read a frame from the video
        capture >> frame;
        if (frame.empty())
        {
            // Video has ended
            break;
        }

        // Serialize the frame
        std::vector<uchar> serializedFrame;
        cv::imencode(".jpg", frame, serializedFrame);

        // Copy the serialized frame to shared memory
        std::memcpy(region.get_address(), serializedFrame.data(), serializedFrame.size());

        // Signal the parent process that a new frame is available
        semaphore.post();
    }
}