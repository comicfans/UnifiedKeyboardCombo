#ifndef UINPUTKEYBOARD_HPP_36CC1JV9
#define UINPUTKEYBOARD_HPP_36CC1JV9


struct libevdev_uinput;

class UinputKeyboard
{
public:

    static UinputKeyboard &instance(){
        static UinputKeyboard s_instance;
        return s_instance;
    }

    void postKeyEvent(unsigned int keyCode,int value);

    UinputKeyboard(const UinputKeyboard& rhs)=delete;
    UinputKeyboard& operator==(const UinputKeyboard& rhs)=delete;

private:

    UinputKeyboard ();

    ~UinputKeyboard();

    libevdev_uinput* m_uinputDev=nullptr;

};



#endif /* end of include guard: UINPUTKEYBOARD_HPP_36CC1JV9 */
