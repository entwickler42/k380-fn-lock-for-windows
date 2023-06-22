#include <iostream>
#include <cstring>
#include <unistd.h>
#include <hidapi.h>


#define MAX_STR 255

int main(int argc, char *argv[])
{
    int seq_len = 7;
    
    const unsigned char k380_seq_fkeys_on[] = {0x10, 0xff, 0x0b, 0x1e, 0x00, 0x00, 0x00};
    const unsigned char k380_seq_fkeys_off[] = {0x10, 0xff, 0x0b, 0x1e, 0x01, 0x00, 0x00};
    
    int k380_vid = 0x46d;
    int k380_pid = 0xb342;
    static const int TARGET_USAGE = 1;
    static const int TARGET_USAGE_PAGE = 65280;
    
    int res;
    int result = -1;
    hid_device *handle;
    struct hid_device_info *devs, *cur_dev;
    
    // Parse command-line options
    int opt;
    bool setMediaKeys = false;
    while ((opt = getopt(argc, argv, "m")) != -1) {
        switch (opt) {
            case 'm':
                setMediaKeys = true;
                break;
            default:
                std::cerr << "Invalid command-line option" << std::endl;
                return 1;
        }
    }
    
    // Initialize the hidapi library
    res = hid_init();
    
    devs = hid_enumerate(k380_vid, k380_pid);
    cur_dev = devs;
    
    while (cur_dev)
    {
        if (cur_dev->usage == TARGET_USAGE && cur_dev->usage_page == TARGET_USAGE_PAGE)
        {
            handle = hid_open_path(cur_dev->path);
            
            if (setMediaKeys)
            {
                std::cout << "Set media keys as default" << std::endl;
                res = hid_write(handle, k380_seq_fkeys_off, seq_len);
            }
            else
            {
                std::cout << "Set function keys as default" << std::endl;
                res = hid_write(handle, k380_seq_fkeys_on, seq_len);
            }
            
            if (res != seq_len){
                std::cout << "error:";
                std::wcout << hid_error(handle) << std::endl;
            }
            
            hid_close(handle);
            if (res < 0){
                result = -1;
                break;
            }else{
                result = 0;
                break;
            }
        }
        cur_dev = cur_dev->next;
    }
    
    hid_free_enumeration(devs);
    hid_exit();
    
    return result;
}
