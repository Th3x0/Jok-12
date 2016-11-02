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

class Monster
{
    private:
        static constexpr int min_frequency=7;
        static constexpr int monster_width=4;
        static constexpr int monster_height=3;
        static constexpr int monster_speed=1;
        static Monster* monsters;
        static int monster_frequency;
        static int counter;
        Monster* next;
        float x;
        float y;
    public:
        Monster(Monster* _next, float _x, float _y):next(_next),x(_x),y(_y)
        {
            std::cout<<"nasco\n";
        }
        ~Monster()
        {
            std::cout<<"muoio\n";
        }
        static void random_gen()//ogni tanto devo aggiungere un nuovo mostro nella catena dinamica di mostri ordinata decrescentemente su y
        {
            if(++counter==monster_frequency)//spazio fra un nemico e l'altro
            {
                counter=0;
                if(rand()%(60+monster_frequency)<=40-monster_frequency)//probabilità di creare un nemico
                {
                    if(rand()%1000<=monster_frequency+60&&monster_frequency>min_frequency)//probabilità di aumentare il numero di nemici
                        --monster_frequency;
                    float this_x=left_limit+(rand()%(129-2*monster_width)+monster_width)*grain;//x iniziale del mostro
                    if(NULL==monsters)//catena vuota
                        monsters=new Monster(NULL,this_x,upper_limit);
                    else //aggiungo in testa
                        monsters=new Monster(monsters,this_x,upper_limit);
                }
            }
        }
        static void move_monsters()
        {
            Monster* temp=monsters;
            for(;temp!=NULL&&temp->next!=NULL&&(temp->next)->next!=NULL;temp=temp->next)//muovo fino al terzultimo (se c'è)
            {
                temp->y-=grain*monster_speed;
            }
            if(temp!=NULL)//elimino quelli che escono dallo schermo
            {
                if(NULL==temp->next)//temp punta all'ultimo (quindi è == monsters)
                {
                    if(temp->y<=lower_limit-monster_height*grain)//devo eliminarlo
                        {
                            delete monsters;
                            monsters=NULL;
                        }
                    else
                        temp->y-=grain*monster_speed;
                }
                else//temp punta al penultimo
                {
                    temp->y-=grain*monster_speed;
                    if((temp->next)->y<=lower_limit-monster_height*grain)
                    {
                        delete temp->next;
                        temp->next=NULL;
                    }
                    else
                        (temp->next)->y-=grain*monster_speed;
                }
            }
        }
        static void draw_monsters()
        {
            glColor3f(0.6,0.2,0.9);
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
        friend bool check_collision(Monster*, Shot*);
        friend void collider();
};

Monster* Monster::monsters=NULL;
int Monster::counter=0;
int Monster::monster_frequency=20;

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
        static void shoot()//devo aggiungere un nuovo shot nella catena dinamica di shots ordinata decrescentemente su y
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
        static void move_shots ()//fa muovere gli shots ed elimina dalla testa se superano il limite
        {
            if (shots!=NULL)
            {
                if(shots->shot_y>=upper_limit)
                {
                    Shot* temp=shots;
                    shots=shots->next;
                    delete temp;
                }
                for(Shot* temp=shots;temp!=NULL;temp=temp->next)
                        temp->shot_y+=grain*shot_speed;
            }
        }
        static void draw_ship()//disegna l'astronave
        {
            glColor3f(0.9,0.9,0.2);
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
        friend bool check_collision(Monster*, Shot*);
        friend void collider();
};

float Spaceship::x=0.0f;
int Spaceship::ship_speed=2;
int Spaceship::shot_speed=4;
Shot* Spaceship::shots=NULL;
int Spaceship::counter=0;

bool check_collision(Monster* mon_temp, Shot* sho_temp)
{
        return (mon_temp->y+Monster::monster_height*grain>=sho_temp->shot_y&&//bordo alto mostro
                mon_temp->y-Monster::monster_height*grain<=sho_temp->shot_y+Spaceship::shot_height&&//bordo basso mostro
                mon_temp->x+Monster::monster_width*grain>=sho_temp->shot_x-grain&&//bordo destro mostro
                mon_temp->x-Monster::monster_width*grain<=sho_temp->shot_x+grain);//bordo sinistro mostro
}

void collider()
{
    if(NULL!=Monster::monsters&&NULL!=Spaceship::shots)
    {
        //controllo il primo mostro E il primo shot
        while(Monster::monsters!=NULL&&
             Spaceship::shots!=NULL&&
             check_collision(Monster::monsters,Spaceship::shots))
        {
            Monster* mon_deleter=Monster::monsters;
            Shot* sho_deleter=Spaceship::shots;
            Monster::monsters=Monster::monsters->next;
            Spaceship::shots=Spaceship::shots->next;
            delete mon_deleter;
            delete sho_deleter;
        }

        if(Monster::monsters!=NULL)//controllo il primo shot con tutti i mostri
        {
            Monster* pre_mon_temp=Monster::monsters;
            Monster* mon_temp=Monster::monsters->next;
            for(;mon_temp!=NULL&&
                 Spaceship::shots!=NULL;mon_temp=mon_temp->next)
            {
                if(check_collision(mon_temp,Spaceship::shots))
                {
                    Monster* mon_deleter=mon_temp;
                    Shot* sho_deleter=Spaceship::shots;
                    pre_mon_temp->next=mon_temp->next;
                    Spaceship::shots=Spaceship::shots->next;
                    delete mon_deleter;
                    delete sho_deleter;
                }
                pre_mon_temp=pre_mon_temp->next;
            }
        }

        if(Spaceship::shots!=NULL)//controllo il primo mostro con tutti gli shot
        {
            Shot* pre_sho_temp=Spaceship::shots;//potrei partire anche dal secondo
            Shot* sho_temp=Spaceship::shots->next;
            for(;Monster::monsters!=NULL&&
                 sho_temp!=NULL;sho_temp=sho_temp->next)
            {
                if(check_collision(Monster::monsters,sho_temp))
                {
                    Monster* mon_deleter=Monster::monsters;
                    Shot* sho_deleter=sho_temp;
                    Monster::monsters=Monster::monsters->next;
                    pre_sho_temp->next=sho_temp->next;
                    delete mon_deleter;
                    delete sho_deleter;
                }
                pre_sho_temp=pre_sho_temp->next;
            }
        }

        if(Spaceship::shots!=NULL&&Monster::monsters!=NULL)//controllo altri mostri e altri shots
        {
            Shot* pre_sho_temp=Spaceship::shots;
            Shot* sho_temp=Spaceship::shots->next;
            Monster* pre_mon_temp=Monster::monsters;
            Monster* mon_temp=Monster::monsters->next;
            while(mon_temp!=NULL&&
                 sho_temp!=NULL)
            {
                if(check_collision(mon_temp,sho_temp))
                {
                    Monster* mon_deleter=mon_temp;
                    Shot* sho_deleter=sho_temp;
                    pre_mon_temp->next=mon_temp->next;
                    pre_sho_temp->next=sho_temp->next;
                    delete mon_deleter;
                    delete sho_deleter;
                }
                if(mon_temp!=NULL&&sho_temp!=NULL)
                {
                    if(mon_temp->y>sho_temp->shot_y)
                    {
                        pre_mon_temp=pre_mon_temp->next;
                        mon_temp=mon_temp->next;
                    }
                    else
                    {
                        pre_sho_temp=pre_sho_temp->next;
                        sho_temp=sho_temp->next;
                    }
                }
            }
        }
    }
}

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
    Spaceship::move_shots();
    Monster::move_monsters();
    collider();
    if(Keyboard_Manager::State('w'))
        Spaceship::shoot();
    Monster::random_gen();
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
