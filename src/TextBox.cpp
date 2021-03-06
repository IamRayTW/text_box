#include "TextBox.h"

using std::vector;
using std::list;
using std::string;

TextBox::TextBox()
{
    col_ = 0;
    row_ = 0;
    x_ = 0;
    y_ = 0;
    base_ = -1;
    font_height_ = 0;
    font_width_ = 0;
    strs_.push_back("");
    left_mouse_key_down_ = false;
    font_size_ = 12;
    hight_light_range_.head.row = 0;
    hight_light_range_.head.col = 0;
    hight_light_range_.tail.row = 0;
    hight_light_range_.tail.col = 0;
}

void TextBox::BuildFont(int height)
{
    if ((int)base_ > 0)
        return;

    HDC hdc = wglGetCurrentDC();
    base_ = glGenLists(256);
    font_height_ = MulDiv(height, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    HFONT font = CreateFontW(
        -font_height_,
        0,                      // 字体宽度
        0,                      // 字体的旋转角度
        0,                      // 字体底线的旋转角度
        FW_THIN,                // 字体重量
        FALSE,                  // 是否使用斜体
        FALSE,                  // 是否使用下划线
        FALSE,                  // 是否使用删除线
        ANSI_CHARSET,           // 设置字符集
        OUT_TT_PRECIS,          // 输出精度
        CLIP_DEFAULT_PRECIS,    // 剪裁精度
        ANTIALIASED_QUALITY,    // 输出质量
        FF_DONTCARE|DEFAULT_PITCH,
        L"細明體");             // 字体名称
    HFONT oldfont = (HFONT)SelectObject(hdc, font);
    wglUseFontBitmaps(hdc, 0, 256, base_);
    GetTextMetrics(hdc, &tm_);

    SIZE tt;
    GetTextExtentPoint32A(wglGetCurrentDC(), "a", 1, &tt);
    font_width_ = tt.cx;
    SelectObject(hdc, oldfont);
    DeleteObject(font);
}

void TextBox::Print(int x, int y)
{
    glPushAttrib(GL_LIST_BIT);
    glListBase(base_ - 0);
   
    for (unsigned int i=0;i<strs_.size();i++)
    {
        HighLight(i);
        glColor3f(0.0f, 0.0f, 0.0f);
        int yy = y + tm_.tmAscent - 1;
        glRasterPos2f(0, yy+ i*tm_.tmHeight);
      //  glCallLists(static_cast<GLsizei>(strs_[i].size()), 
     //       GL_UNSIGNED_BYTE, strs_[i].c_str());

        for (int j = 0;j <strs_[i].size();j++)
            glCallList(base_ + strs_[i][j]);
    }

    //printf("%d\n", content_.size());
    //int i =0;
    //glColor3f(0.0f, 0.0f, 0.0f);
    //int yy = y + tm_.tmAscent - 1;
    //glRasterPos2f(0, yy+ i*tm_.tmHeight);
    //list<char>::iterator iter = content_.begin();
    //for (iter;iter!=content_.end();++iter)
    //{
    //    glCallList(base_ + *iter);
    //}

    glPopAttrib();
}

void TextBox::HighLight(unsigned int i)
{
    glColor3f(51/255.0, 143/255.0, 1);

    int sss, sss2;
    int bbb, bbb2;
    if (first_down_row_ <= last_down_row_)
    {
        sss = first_down_row_;
        bbb = last_down_row_;
        sss2 = first_down_col_;
        bbb2 = last_down_col_;
    }
    else
    {
        sss = last_down_row_;
        bbb = first_down_row_;
        sss2 = last_down_col_;
        bbb2 = first_down_col_;
    }

    is_high_light_ = true;

    if (sss2 == bbb2)
    {
        is_high_light_ = false;
    }
    else if (sss == i)
    {
        unsigned int ll;
        if (bbb == i)
            ll = last_down_col_;
        else
            ll = static_cast<unsigned int>(strs_[i].size());

        glBegin(GL_QUADS);
        glVertex3f(sss2*font_width_, i*font_height_, 0);
        glVertex3f(ll*font_width_, i*font_height_, 0);
        glVertex3f(ll*font_width_, (i+1)*font_height_, 0);
        glVertex3f(sss2*font_width_, (i+1)*font_height_, 0);
        glEnd();
    }
    else if (bbb == i)
    {
        int ff;
        if (bbb == i)
            ff = bbb2;
        else
            ff = static_cast<int>(strs_[i].size());

        glBegin(GL_QUADS);
        glVertex3f(0*font_width_, i*font_height_, 0);
        glVertex3f(bbb2*font_width_, i*font_height_, 0);
        glVertex3f(bbb2*font_width_, (i+1)*font_height_, 0);
        glVertex3f(0*font_width_, (i+1)*font_height_, 0);
        glEnd();
    }
    else if (sss < i && i < bbb)
    {
        int rr = static_cast<int>(strs_[i].size());
        glBegin(GL_QUADS);
        glVertex3f(0, i*font_height_, 0);
        glVertex3f(rr*font_width_, i*font_height_, 0);
        glVertex3f(rr*font_width_, (i+1)*font_height_, 0);
        glVertex3f(0, (i+1)*font_height_, 0);
        glEnd();
    }
    else
    {
        is_high_light_ = false;
    }
}

int TextBox::OnKey(unsigned int key, unsigned int state)
{
    if (state == 1)
    {
        SystemChar(key);
        return 0;
    }

    if (key == '\t')
    {
        AddChar(' ');
        AddChar(' ');
        AddChar(' ');
        AddChar(' ');
    }
    else
        AddChar(key);
    return 0;
}

int TextBox::OnMouse(short x, 
                     short y, 
                     unsigned char is_down, 
                     int bits)
{
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    x_ = x;
    y_ = y;

    if (is_down != 2)
    {
        if (is_down == 1)
            left_mouse_key_down_ = true;
        else
            left_mouse_key_down_ = false;
    }

    if (is_down == 1)
    {
        int yy = y / font_height_;
        if (yy <= static_cast<int>(strs_.size()-1))
            row_ = yy;
        else
            row_ = static_cast<int>(strs_.size() - 1);

        int xx = (x + (font_width_ >> 1)) / font_width_;

        if (xx <= static_cast<int>(strs_[row_].size()))
            col_ = xx;
        else
            col_ = static_cast<int>(strs_[row_].size());

        first_down_row_ = row_;
        first_down_col_ = col_;
    }

    if (left_mouse_key_down_ == true)
    {
        int cc, rr;
        int yy = y / font_height_;
        if (yy <= static_cast<int>(strs_.size()-1))
            cc = yy;
        else
            cc = static_cast<int>(strs_.size() - 1);

        int xx = (x + (font_width_ >> 1)) / font_width_;

        if (xx <= static_cast<int>(strs_[cc].size()))
            rr = xx;
        else
            rr = static_cast<int>(strs_[cc].size());

        last_down_row_ = cc;
        last_down_col_ = rr;
    }

    return 0;
}

int TextBox::Work()
{
    unsigned int width = 0;
    unsigned int height = 0;

    this->GetWidthHeight(width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width-1, height-1, 0, 1,-1);
    glClearColor (1, 1, 1, 1);
    glClear(  GL_COLOR_BUFFER_BIT 
            | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_POINTS);
    for (int i=0;i<=font_height_;i++)
    {
        glVertex3f(0 + font_width_*col_, i + font_height_*row_, 0);
        glColor3f(1,0,0);
    }
    glEnd();

    BuildFont(font_size_);
    Print(0, 0);

    return 0;
}

void TextBox::SystemChar(char key)
{
    printf("key %d\n", key);

    if (key == 35)
    {
        col_ = strs_[row_].size();
    }
    else if (key == 36)
    {
        col_ = 0;
    }
    else if (key == 37)
    {
        if (col_ != 0)
            col_--;
        else
        {
            if (row_ != 0)
            {
                row_--;
                col_ = strs_[row_].size();
            }
        }
    }
    else if (   key == 38
             && row_ != 0)
    {
        row_--;
        if (col_ >= strs_[row_].size())
            col_ = strs_[row_].size();
    }
    else if (key == 39)
    {
        if (col_ < strs_[row_].size())
            col_++;
        else
        {
            if (row_ < strs_.size()-1)
            {
                row_++;
                col_ = 0;
            }
        }
    }
    else if (key == 40)
    {
        if (row_+1 < strs_.size())
        {
            row_++;
            if (col_ >= strs_[row_].size())
                col_ = strs_[row_].size();
        }
        else
        {
            row_ = strs_.size()-1;
        }
    }
    else if (key == 46)
    {
        if (col_ < strs_[row_].size())
        {
            strs_[row_].erase(strs_[row_].begin()+col_);
        }
        else
        {
            if (row_ < strs_.size()-1)
            {
                strs_[row_] += strs_[row_+1];
                strs_.erase(strs_.begin()+row_+1);
            }
        }
    }
}

void TextBox::DoBackSpace()
{
    printf("is_high_light_ %d\n", is_high_light_);
    if (col_ == 0)
    {
        if (row_ == 0)
            return;

        col_ = static_cast<int>(strs_[row_-1].size());
        strs_[row_-1] += strs_[row_];
        strs_.erase(strs_.begin()+row_);
        row_--;

        return;
    }

    strs_[row_].erase(strs_[row_].begin() + col_-1);
    col_--;
}

void TextBox::AddChar(char key)
{
    if (key == 8)
    {
        DoBackSpace();
        return;
    }

    if (key == '\r')
    {
        strs_.insert(strs_.begin()+row_+1, strs_[row_].substr(col_));
        strs_[row_].erase(col_);
        row_++;
        col_ = 0;
        return;
    }

    if (this->is_high_light_ == true)
    {
        printf("sjhot\n");
    }

    strs_[row_].insert(strs_[row_].begin()+col_, key);
    col_++;

    content_.push_back(key);
}

void TextBox::SetFontSize(unsigned int size)
{
    font_size_ = size;
}