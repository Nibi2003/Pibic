#include <GL/glut.h>
#include <cmath> // Para usar fabs
#include <map>
#include <vector>
#include <string>
#include <sstream>

// Variável global float
float larguraOmbros = 0.50;

float metadeLarguaOmbros = larguraOmbros/2;

// Variável global float
float distanciaPeitoralCelular = 0.30;


struct Point {
    float x, y;
    Point(float x, float y) : x(x), y(y) {}
};

// Mapa dos beacons com suas coordenadas
std::map<std::string, Point> beacons = {
    {"b1", Point(3.9, 0.0)},
    {"b2", Point(0.0, 3.1)},
    {"b3", Point(7.9, 3.1)},
    {"b4", Point(0.0, 7.0)},
    {"b5", Point(7.9, 7.0)},
    {"b6", Point(0.0, 11.4)},
    {"b7", Point(7.9, 11.4)},
    {"b8", Point(3.9, 14.1)},
    {"b9", Point(2.9, 14.1)}
};

// Vetor dos pontos adicionais a serem desenhados
std::vector<Point> additional_points = {
    {6.27, 3.76}, {3.92, 8.75}, {1.57, 13.74}, {1.57, 8.75}, {1.57, 7.09},
    {6.27, 7.09}, {1.57, 10.42}, {1.57, 5.42}, {3.92, 5.42}, {3.92, 13.74},
    {6.27, 12.08}, {1.57, 2.03}, {3.92, 3.76}, {1.57, 3.76}, {6.27, 8.75},
    {3.92, 12.08}, {3.92, 0.77}, {3.92, 7.09}, {1.57, 0.77}, {1.57, 12.08},
    {3.92, 10.42}, {6.27, 5.42}, {6.27, 2.03}, {3.92, 2.03}, {6.27, 13.74},
    {6.27, 10.42}, {6.27, 0.77}
};

// Ponto atual do programador
Point currentProgrammerPosition(-1.0, -1.0);
// Ponto do beacon selecionado
Point selectedBeaconPosition(-1.0, -1.0);
// Ponto do celular do programador
Point currentPhonePosition(-1.0, -1.0);

// Direção do programador (1: Norte, 2: Leste, 3: Sul, 4: Oeste)
int programmerDirection = 1; // Começa apontando para o Norte


void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);

    // Loop para desenhar cada caractere do texto
    for (size_t i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
}



// Função para desenhar o programador e seu celular com base na direção
void drawProgrammer(float x, float y, float phoneX, float phoneY, int direction) {
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINES);
    if (direction == 1 || direction == 3) { // Norte ou Sul
        glVertex2f(x - metadeLarguaOmbros, y);
        glVertex2f(x + metadeLarguaOmbros, y);
    } else if (direction == 2 || direction == 4) { // Leste ou Oeste
        glVertex2f(x, y - metadeLarguaOmbros);
        glVertex2f(x, y + metadeLarguaOmbros);
    }
    glEnd();
    glColor3f(1.0, 0.5, 0.0); // Laranja forte para o ponto do celular
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex2f(phoneX, phoneY);
    glEnd();
}

// Função para calcular a interseção entre dois segmentos de linha
bool lineSegmentsIntersect(Point p1, Point p2, Point p3, Point p4) {
    float Ax = p1.x, Ay = p1.y;
    float Bx = p2.x, By = p2.y;
    float Cx = p3.x, Cy = p3.y;
    float Dx = p4.x, Dy = p4.y;

    float denominator = ((Bx - Ax) * (Dy - Cy)) - ((By - Ay) * (Dx - Cx));
    if (denominator == 0) {
        return false; // Os segmentos são paralelos
    }

    float numerator1 = ((Ay - Cy) * (Dx - Cx)) - ((Ax - Cx) * (Dy - Cy));
    float numerator2 = ((Ay - Cy) * (Bx - Ax)) - ((Ax - Cx) * (By - Ay));

    float r = numerator1 / denominator;
    float s = numerator2 / denominator;

    if (r >= 0 && r <= 1 && s >= 0 && s <= 1) {
        return true; // Os segmentos se interceptam
    }

    return false; // Os segmentos não se interceptam
}

void display() {
    glClearColor(0.9, 0.9, 0.8, 1.0); // Define a cor de fundo como bege
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Ajusta a projeção para dar um pouco de zoom
    glOrtho(-1.0, 8.9, -1.0, 15.1, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Desenha o retângulo principal
    glColor3f(1.0, 1.0, 1.0); // Define a cor do retângulo como branca
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 0.0);      // Canto inferior esquerdo
    glVertex2f(7.9, 0.0);      // Canto inferior direito
    glVertex2f(7.9, 14.1);     // Canto superior direito
    glVertex2f(0.0, 14.1);     // Canto superior esquerdo
    glEnd();

    // Desenha os beacons como pontos pretos
    glColor3f(0.0, 0.0, 0.0); // Define a cor dos beacons como preta
    glPointSize(5.0); // Define o tamanho dos pontos
    glBegin(GL_POINTS);
    for (const auto& beacon : beacons) {
        glVertex2f(beacon.second.x, beacon.second.y);
    }
    glEnd();
    // Desenha as labels dos beacons
    glColor3f(0.0, 0.0, 0.0); // Define a cor do texto como preto
    for (const auto& beacon : beacons) {
        drawText(beacon.second.x + 0.1, beacon.second.y + 0.1, beacon.first);
    }

    // Desenha os pontos adicionais como pontos azuis
    glColor3f(0.0, 0.0, 1.0); // Define a cor dos pontos adicionais como azul
    glPointSize(5.0); // Define o tamanho dos pontos
    glBegin(GL_POINTS);
    for (const auto& point : additional_points) {
        glVertex2f(point.x, point.y);
    }
    glEnd();

    // Desenha o texto dos pontos adicionais
    for (const auto& point : additional_points) {
        std::stringstream ss;
        glColor3f(0.8, 0.8, 0.8); // Define a cor do texto como cinza claro
        ss << "(" << point.x << ", " << point.y << ")";
        drawText(point.x, point.y + 0.1, ss.str()); // Desenha o texto um pouco acima do ponto
    }


    // Desenha o programador na posição atual com base na direção
    if (currentProgrammerPosition.x != -1.0 && currentProgrammerPosition.y != -1.0 &&
        currentPhonePosition.x != -1.0 && currentPhonePosition.y != -1.0) {
        drawProgrammer(currentProgrammerPosition.x, currentProgrammerPosition.y,
                       currentPhonePosition.x, currentPhonePosition.y, programmerDirection);
    }


    // Desenha a linha do beacon selecionado até a posição do programador, se houver
    // Desenha a linha do beacon selecionado até a posição do programador, se houver
    if (selectedBeaconPosition.x != -1.0 && selectedBeaconPosition.y != -1.0 &&
        currentPhonePosition.x != -1.0 && currentPhonePosition.y != -1.0) {
        glColor3f(1.0, 0.0, 0.0); // Vermelho para a linha do beacon
        glBegin(GL_LINES);
        glVertex2f(selectedBeaconPosition.x, selectedBeaconPosition.y);
        glVertex2f(currentPhonePosition.x, currentPhonePosition.y);
        glEnd();

        // Define os pontos p1 e p2 do segmento do programador baseado na direção
        Point p1(currentProgrammerPosition.x, currentProgrammerPosition.y); // Ponto inicial igual à posição do programador

        if (programmerDirection == 1 || programmerDirection == 3) { // Norte ou Sul
            p1.x -= metadeLarguaOmbros;
        } else if (programmerDirection == 2 || programmerDirection == 4) { // Leste ou Oeste
            p1.y -= metadeLarguaOmbros;
        }

        Point p2(currentProgrammerPosition.x, currentProgrammerPosition.y); // Ponto final igual à posição do programador

        if (programmerDirection == 1 || programmerDirection == 3) { // Norte ou Sul
            p2.x += metadeLarguaOmbros;
        } else if (programmerDirection == 2 || programmerDirection == 4) { // Leste ou Oeste
            p2.y += metadeLarguaOmbros;
        }

        Point p3(selectedBeaconPosition.x, selectedBeaconPosition.y);
        Point p4(currentPhonePosition.x, currentPhonePosition.y);

        bool intersects = lineSegmentsIntersect(p1, p2, p3, p4);

        // Desenha o texto correspondente ao resultado da interseção
        std::string popupText = intersects ? "INTERCEPTA" : "NAO INTERCEPTA";
        drawText(selectedBeaconPosition.x + 0.1, selectedBeaconPosition.y + 0.1, popupText);
    }

    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLdouble modelview[16];
        GLdouble projection[16];
        GLint viewport[4];
        GLdouble winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        winX = (double)x;
        winY = (double)viewport[3] - (double)y;
        glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &winZ);

        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        // Verifica se o clique está próximo a algum dos pontos adicionais
        for (const auto& point : additional_points) {
            if (fabs(point.x - posX) < 0.5 && fabs(point.y - posY) < 0.5) {
                currentProgrammerPosition = point;
                currentPhonePosition = Point(point.x, point.y + distanciaPeitoralCelular);  // Define a posição do ponto laranja
                selectedBeaconPosition = Point(-1.0, -1.0); // Reseta o beacon selecionado
                programmerDirection = 1;
                glutPostRedisplay(); // Re-desenha a cena
                return;
            }
        }

        // Verifica se o clique está próximo a algum dos beacons
        for (const auto& beacon : beacons) {
            if (fabs(beacon.second.x - posX) < 0.5 && fabs(beacon.second.y - posY) < 0.5) {
                selectedBeaconPosition = beacon.second;
                glutPostRedisplay(); // Re-desenha a cena
                return;
            }
        }
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            programmerDirection = 1; // Norte
            currentPhonePosition = Point(currentProgrammerPosition.x, currentProgrammerPosition.y + distanciaPeitoralCelular);
            glutPostRedisplay(); // Redesenha a cena após mudar a direção para Norte
            break;
        case GLUT_KEY_RIGHT:
            programmerDirection = 2; // Leste
            currentPhonePosition = Point(currentProgrammerPosition.x + distanciaPeitoralCelular, currentProgrammerPosition.y);
            glutPostRedisplay(); // Redesenha a cena após mudar a direção para Leste
            break;
        case GLUT_KEY_DOWN:
            programmerDirection = 3; // Sul
            currentPhonePosition = Point(currentProgrammerPosition.x, currentProgrammerPosition.y - distanciaPeitoralCelular);
            glutPostRedisplay(); // Redesenha a cena após mudar a direção para Sul
            break;
        case GLUT_KEY_LEFT:
            programmerDirection = 4; // Oeste
            currentPhonePosition = Point(currentProgrammerPosition.x - distanciaPeitoralCelular, currentProgrammerPosition.y);
            glutPostRedisplay(); // Redesenha a cena após mudar a direção para Oeste
            break;
    }
}



// Função principal
int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Inicializa GLUT
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Configura modo de exibição
    glutInitWindowSize(800, 800);           // Configura tamanho da janela
    glutCreateWindow("Laboratório");        // Cria janela com título

    glutDisplayFunc(display);               // Define a função de desenho
    glutMouseFunc(mouse);                   // Define a função de mouse
    glutSpecialFunc(specialKeys);           // Define a função de teclas especiais (setas)

    glutMainLoop();                         // Inicia loop de eventos GLUT

    return 0;
}