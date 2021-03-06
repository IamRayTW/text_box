#include "ruser_interface.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <list>
#include <vector>

class TextBox : public RUserInterface
{
    struct RowColumn
    {
        unsigned int row;
        unsigned int col;
    };

    struct HighLightRange
    {
        RowColumn head;
        RowColumn tail;
    };

    int col_;
    int row_;
    int x_;
    int y_;
    std::vector<std::string> strs_;
    std::list<char> content_;
    GLuint base_;
    TEXTMETRIC tm_;
    int font_height_;
    int font_width_;
    bool left_mouse_key_down_;
    unsigned int first_down_row_;
    unsigned int first_down_col_;
    unsigned int last_down_row_;
    unsigned int last_down_col_;
    HighLightRange hight_light_range_;
    bool is_high_light_;
    unsigned int font_size_;

public:
    TextBox();
    void SetText(const std::string& text);
    void GetText(std::string& text);
    void SetFontSize(unsigned int size);

private:
    virtual int OnMouse(short x, short y,unsigned char is_down, int bits);
    virtual int OnKey(unsigned int key, unsigned int state);
    virtual int Work();

    void SystemChar(char c);
    void DoBackSpace();
    void AddChar(char c);
    void BuildFont(int height);
    void Print(int x, int y);
    void HighLight(unsigned int row);
};
