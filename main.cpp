#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>

//GRAPHIC_CONST
static constexpr float grain = 0.015625f;
static constexpr float lower_limit = -1.0f;
static constexpr float upper_limit = 1.0f;
static constexpr float left_limit = -1.0f;
static constexpr float right_limit = 1.0f;

// CLASSI


struct Shot
{
    Shot* next;
    float shot_x;
    float shot_y;
    Shot(Shot* _next,float _x, float _y):next(_next),shot_x(_x),shot_y(_y)
    {}
    ~Shot()
    {}
};

class Spaceship
{
    private:
        static float x;
        static int ship_speed;
        static int shot_speed;
        static int counter;
        static Shot* shots;
        static constexpr float y=grain*12;
        static constexpr float height=grain*8;
        static constexpr float width=grain*3;
        static constexpr float shot_height=grain*2;
        static constexpr int shot_frequency=15;

    public:
        static void move_left()//muove l'astronave a sinistra
        {
            if(x>right_limit-width||x<left_limit-width)
                x=0.0f;
            if(x>=left_limit+width+grain*ship_speed)
                x-=grain*ship_speed;
        }
        static void move_right()//muove l'astronave a destra
        {
            if(x>right_limit-width||x<left_limit+width)
                x=0.0f;
            if(x<=right_limit-width-grain*ship_speed)
                x+=grain*ship_speed;
        }
        static void shoot()//devo aggiungere un nuovo shot nella catena dinamica di shots
        {
            if(++counter>=shot_frequency)//limito la freqenza massima di sparo
            {
                counter=0;
                if(NULL==shots)//catena vuota
                    shots=new Shot(NULL,x,lower_limit+y);
                else
                {
                    Shot* temp=shots;
                    for(;temp->next!=NULL;temp=temp->next)//aggiungo in fondo
                    {}
                    temp->next=new Shot(NULL,x,lower_limit+y);
                }
            }
        }
        static void move_shots ()//fa muovere gli shots ed elimina dal fondo se superano il limite
        {
            if (shots!=NULL)
            {
                if(shots->next!=NULL)
                {
                    Shot* temp=shots;
                    for(;(temp->next)->next!=NULL;temp=temp->next)
                            temp->shot_y+=grain*shot_speed;
                    temp->shot_y+=grain*shot_speed;
                    if((temp->next)->shot_y<upper_limit)
                        (temp->next)->shot_y+=grain*shot_speed;
                    else
                    {
                        delete temp->next;
                        temp->next=NULL;
                    }
                }
                else if (shots->shot_y<upper_limit)
                    shots->shot_y+=grain*shot_speed;
                else
                {
                    delete shots;
                    shots=NULL;
                }
            }
        }
        static void draw_ship()//disegna l'astronave
        {
            glColor3f(1,1,1);
            glBegin(GL_TRIANGLES);
            glVertex2f(x,lower_limit+y);
            glVertex2f(x-width,lower_limit+y-height);
            glVertex2f(x+width,lower_limit+y-height);
            glEnd();
        }
        static void draw_shots()//disegna gli shots
        {
            glColor3f(1,1,1);
            glBegin(GL_TRIANGLES);
            for(Shot* temp=shots;temp!=NULL;temp=temp->next)
            {
                glVertex2f(temp->shot_x,temp->shot_y);
                glVertex2f(temp->shot_x+grain,temp->shot_y+shot_height);
                glVertex2f(temp->shot_x-grain,temp->shot_y+shot_height);
            }
            glEnd();
        }
};

float Spaceship::x=0.0f;
int Spaceship::ship_speed=2;
int Spaceship::shot_speed=4;
Shot* Spaceship::shots=NULL;
int Spaceship::counter=0;

class Monster
{
    private:
        static Monster* monsters;
        static int monster_frequency;
        static int counter;
        static int monster_width;
        static int monster_height;
        static int monster_speed;
        Monster* next;
        float x;
        float y;
    public:
        Monster(Monster* _next, float _x, float _y):next(_next),x(_x),y(_y)
        {
        }
        static void random_gen()
        {
            if(++counter==monster_frequency)
            {
                counter=0;
                if(rand()%(100-monster_frequency)<=monster_frequency)
                {
                    if(rand()%100<=2)
                        --monster_frequency;
                    float this_x=left_limit+(rand()%129-2*monster_width)*grain;
                    if(NULL==monsters)//catena vuota
                        monsters=new Monster(NULL,this_x,upper_limit);
                    else
                    {
                        Monster* temp=monsters;
                        for(;temp->next!=NULL;temp=temp->next)//aggiungo in fondo
                        {}
                        temp->next=new Monster(NULL,this_x,upper_limit);
                    }
                }
            }
        }
        static void draw_monsters()
        {
            glColor3f(1,1,1);
            glBegin(GL_QUADS);
            for(Monster* temp=monsters;temp!=NULL;temp=temp->next)
            {
                glVertex2f(temp->x+monster_width*grain,temp->y+monster_height*grain);
                glVertex2f(temp->x-monster_width*grain,temp->y+monster_height*grain);
                glVertex2f(temp->x-monster_width*grain,temp->y-monster_height*grain);
                glVertex2f(temp->x+monster_width*grain,temp->y-monster_height*grain);
            }
            glEnd();
        }
        static move_monsters()
        {
            for(Monster* temp=monsters;temp!=NULL;temp=temp->next)
            {
                temp->y-=grain*monster_speed;
            }
        }
};

Monster* Monster::monsters=NULL;
int Monster::counter=0;
int Monster::monster_frequency=20;
int Monster::monster_width=2;
int Monster::monster_height=3;
int Monster::monster_speed=1;

class Keyboard_Manager
{
    private:
        static bool Keys[256];
    public:
        static void Up(char key)
        {
            Keys[key]=FALSE;
        }

        static void Down(char key)
        {
            Keys[key]=TRUE;
        }

        static bool State(char key)
        {
            return Keys[key];
        }
};

bool Keyboard_Manager::Keys[]={};

/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Spaceship::draw_ship();
    Spaceship::draw_shots();
    Monster::draw_monsters();
    glFlush();
}

static void key(unsigned char key, int x, int y)
{
    if(key=='q'||key==27)
        exit(0);
    Keyboard_Manager::Down(key);
    if(key=='w')
        Spaceship::shoot();
    glutPostRedisplay();
}

static void keyUp(unsigned char key, int x, int y)
{
    Keyboard_Manager::Up(key);
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

static void timer(int useless)
{
    if(Keyboard_Manager::State('d'))
        Spaceship::move_right();
    if(Keyboard_Manager::State('a'))
        Spaceship::move_left();
    if(Keyboard_Manager::State('w'))
        Spaceship::shoot();
    Spaceship::move_shots();
    Monster::random_gen();
    Monster::move_monsters();
    glutPostRedisplay();
    glutTimerFunc(16,timer,0);
}

/* Program entry point */

int main(int argc, char *argv[])
{

    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(700,10);
    glutInitDisplayMode(GLUT_RGB );
    glutCreateWindow("Jok 12");
    //glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);
    glutTimerFunc(16,timer,0);

    glutMainLoop();

    return EXIT_SUCCESS;
}
