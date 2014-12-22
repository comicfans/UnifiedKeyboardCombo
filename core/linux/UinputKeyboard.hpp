#ifndef UINPUTKEYBOARD_HPP_36CC1JV9
#define UINPUTKEYBOARD_HPP_36CC1JV9

#include <libevdev/libevdev.h>

#include <string>

struct libevdev_uinput;

class UinputKeyboard
{
public:


    enum MouseRelAction{
        MOUSE_X_NEG=KEY_CNT,MOUSE_X_POS,
        MOUSE_Y_NEG,MOUSE_Y_POS,
        MOUSE_HWHEEL_NEG,MOUSE_HWHEEL_POS,
        MOUSE_WHEEL_NEG,MOUSE_WHEEL_POS,
        MOUSE_DPI_NEG,MOUSE_DPI_POS,
        MOUSE_REL_TYPE_COUNT=10}; 

    static constexpr const char * const VIRTUAL_DEVICE_PREFIX="UnifiedKeyboardCombo:";

    static constexpr const char * const VIRTUAL_KEYBOARD_NAME="UnifiedKeyboardCombo:virtual keyboard";

    static int mouseRelActionNameToCode(const std::string& name); 

    static UinputKeyboard &instance(){
        static UinputKeyboard s_instance;
        return s_instance;
    }

    void postKeyEvent(unsigned int keyCode,int value);

    void postMouseEvent(MouseRelAction mouseRelAction); 

    UinputKeyboard(const UinputKeyboard& rhs)=delete;
    UinputKeyboard& operator==(const UinputKeyboard& rhs)=delete;

private:

    UinputKeyboard ();

    ~UinputKeyboard();

    libevdev_uinput* m_uinputDev=nullptr;

    uint16_t m_virtualDpiFactor=8; 

};



#endif /* end of include guard: UINPUTKEYBOARD_HPP_36CC1JV9 */
