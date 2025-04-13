#include "utils.hpp"

class Canvas {
    public:
    virtual void drawpixel(int y, int x, int color) {}
};

class SubCanvas : public Canvas {
    protected:
    Canvas *canvas;
    int x, y;
    public:
    SubCanvas(Canvas *canvas, int y, int x) {
        this->canvas = canvas;
        this->x = x;
        this->y = y;
    }
    void drawpixel(int y, int x, int color) {
        canvas->drawpixel(this->y + y, this->x + x, color);
    }
};

namespace TBlock {
    struct bmtx {
        bool mask[4][4];
    };
    
    const bmtx BMASK[] = {
        {{ // I 0
            {1, 1, 1, 1}
        }},
        {{ // I 1
            {1},
            {1},
            {1},
            {1}
        }},
        {{ // J 0
            {1},
            {1, 1, 1}
        }},
        {{ // J 1
            {1, 1},
            {1},
            {1}
        }},
        {{ // J 2
            {1, 1, 1},
            {0, 0, 1}
        }},
        {{ // J 3
            {0, 1},
            {0, 1},
            {1, 1}
        }},
        {{ // L 0
            {0, 0, 1},
            {1, 1, 1}
        }},
        {{ // L 1
            {1},
            {1},
            {1, 1}
        }},
        {{ // L 2
            {1, 1, 1},
            {1}
        }},
        {{ // L 3
            {1, 1},
            {0, 1},
            {0, 1}
        }},
        {{ // O
            {1, 1},
            {1, 1}
        }},
        {{ // S 0
            {0, 1, 1},
            {1, 1}
        }},
        {{ // S 1
            {1},
            {1, 1},
            {0, 1}
        }},
        {{ // T 0
            {0, 1},
            {1, 1, 1}
        }},
        {{ // T 1
            {1},
            {1, 1},
            {1}
        }},
        {{ // T 2
            {1, 1, 1},
            {0, 1}
        }},
        {{ // T 3
            {0, 1},
            {1, 1},
            {0, 1}
        }},
        {{ // Z 0
            {1, 1},
            {0, 1, 1}
        }},
        {{ // Z 0
            {0, 1},
            {1, 1},
            {1}
        }}
    };
    const bmtx *BOMASK[] = {
        // I
        BMASK,
        // J
        BMASK + 2,
        // L
        BMASK + 6,
        // O
        BMASK + 10,
        // S
        BMASK + 11,
        // T
        BMASK + 13,
        // Z
        BMASK + 17
    };
    enum {
        I = 0,
        J = 1,
        L = 2,
        O = 3,
        S = 4,
        T = 5,
        Z = 6,
        n_B = 7
    };
    void draw(Canvas *canvas, int y, int x, int b, int o) {
        auto ptr = BOMASK[b]+o;
        for (int i=0;i<4;i++) {
            for (int j=0;j<4;j++) {
                if (ptr->mask[i][j])
                    canvas->drawpixel(i+y, j+x, b+1);
            }
        }
    }
    void draw(char buf_space[20][10], int y, int x, int b, int o) {
        auto ptr = BOMASK[b]+o;
        for (int i=0;i<4;i++) {
            for (int j=0;j<4;j++) {
                if (ptr->mask[i][j])
                    buf_space[i+y][j+x] = b+1;
            }
        }
    }
    bool is_collide(char buf_space[20][10], int y, int x, int b, int o) {
        auto ptr = BOMASK[b]+o;
        for (int i=0;i<4;i++) {
            for (int j=0;j<4;j++) {
                if (ptr->mask[i][j]) {
                    if (!inrange(i+y, 0, 19) || !inrange(j+x, 0, 9))
                        return true;
                    if (buf_space[i+y][j+x])
                        return true;
                }
            }
        }
        return false;
    }
    int max_ori(int b) {
        switch (b) {
            case I:
            case S:
            case Z:
                return 2;
            case J:
            case L:
            case T:
                return 4;
            case O:
                return 1;
        }
        return 0;
    }
};