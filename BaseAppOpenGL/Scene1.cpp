#include "Scene1.h"
#include <vector>


CScene1::CScene1()
{

	pCamera = NULL;
	pTexto = NULL;

	bIsWireframe = false;
	bIsCameraFPS = true;

	iFPS = 0;
	iFrames = 0;
	ulLastFPS = 0;
	szTitle[256] = 0;

	// Cria gerenciador de impressão de texto na tela
	pTexto = new CTexto();

	// Cria camera
	pCamera = new CCamera(0.0f, 1.0f, 20.0f);

	pTexture = new CTexture();	
	pTexture->CreateTextureLinear(0, "../Scene1/CRATE.BMP");
	pTexture->CreateTextureLinear(1, "../Scene1/brick2.bmp");
	pTexture->CreateTextureLinear(2, "../Scene1/grass.bmp");


	// Cria o Timer
	pTimer = new CTimer();
	pTimer->Init();

	fTimerPosY = 0.0f;
	fRenderPosY = 0.0f;

	fPosX = 0.0f;
	fPosY = 0.0f;
	fPosZ = 0.0f;
	fMovementFactor = 0.1f;

	fRotX = 0.0f;
	fRotY = 1.0f;
	fRotZ = 0.0f;
	fAngle = 0.0f;
}


CScene1::~CScene1(void)
{
	if (pTexto)
	{
		delete pTexto;
		pTexto = NULL;
	}

	if (pCamera)
	{
		delete pCamera;
		pCamera = NULL;
	}

	if (pTimer)
	{
		delete pTimer;
		pTimer = NULL;
	}

	if (pTexture) 
	{
		delete pTexture;
		pTexture = NULL;
	}
}




int CScene1::DrawGLScene(void)	// Função que desenha a cena
{
	// Get FPS
	if (GetTickCount() - ulLastFPS >= 1000)	// When A Second Has Passed...
	{
		ulLastFPS = GetTickCount();				// Update Our Time Variable
		iFPS = iFrames;							// Save The FPS
		iFrames = 0;							// Reset The FPS Counter
	}
	iFrames++;									// FPS counter
	
	pTimer->Update();							// Atualiza o timer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Limpa a tela e o Depth Buffer
	glLoadIdentity();									// Inicializa a Modelview Matrix Atual


	// Seta as posições da câmera
	pCamera->setView();

	// Desenha grid 
	Draw3DSGrid(20.0f, 20.0f);

	DrawAxis();

	// Modo FILL ou WIREFRAME (pressione barra de espaço)	
	if (bIsWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	glEnable(GL_TEXTURE_2D);	
	glColor3f(1.0f, 1.0f, 1.0f);
	vector<Quadrado> qQuadrados = LerArquivo("../Cubos.obj");
	for (int i = 0; i < qQuadrados.size(); i++) {
		Quadrado q = qQuadrados[i];
		glPushMatrix();
		pTexture->ApplyTexture(0);
		glBegin(GL_QUADS);
			glTexCoord3d(q.textura1.x, q.textura1.y, q.textura1.z); glVertex3f(q.vertice1.x, q.vertice1.y, q.vertice1.z);
			glTexCoord3d(q.textura2.x, q.textura2.y, q.textura2.z); glVertex3f(q.vertice2.x, q.vertice2.y, q.vertice2.z);
			glTexCoord3d(q.textura3.x, q.textura3.y, q.textura3.z); glVertex3f(q.vertice3.x, q.vertice3.y, q.vertice3.z);
			glTexCoord3d(q.textura4.x, q.textura4.y, q.textura4.z); glVertex3f(q.vertice4.x, q.vertice4.y, q.vertice4.z);
		glEnd();
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (FIM)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	fTimerPosY = pTimer->GetTime() / 1000.0f;
	fRenderPosY += 0.2f;

	// Impressão de texto na tela...
	// Muda para modo de projeção ortogonal 2D
	// OBS: Desabilite Texturas e Iluminação antes de entrar nesse modo de projeção
	OrthoMode(0, 0, WIDTH, HEIGHT);


	glPushMatrix();
	glTranslatef(0.0f, HEIGHT - 100, 0.0f);	// Move 1 unidade para dentro da tela (eixo Z)

	// Cor da fonte
	glColor3f(1.0f, 1.0f, 0.0f);


	glRasterPos2f(10.0f, 0.0f);	// Posicionando o texto na tela
	if (!bIsWireframe) {
		pTexto->glPrint("[TAB]  Modo LINE"); // Imprime texto na tela
	}
	else {
		pTexto->glPrint("[TAB]  Modo FILL");
	}


	//// Camera LookAt
	glRasterPos2f(10.0f, 40.0f);
	pTexto->glPrint("Player LookAt  : %f, %f, %f", pCamera->Forward[0], pCamera->Forward[1], pCamera->Forward[2]);

	//// Posição do Player
	glRasterPos2f(10.0f, 60.0f);
	pTexto->glPrint("Player Position: %f, %f, %f", pCamera->Position[0], pCamera->Position[1], pCamera->Position[2]);

	//// Imprime o FPS da aplicação e o Timer
	glRasterPos2f(10.0f, 80.0f);
	pTexto->glPrint("Frames-per-Second: %d ---- Timer: %.1f segundos", iFPS, (pTimer->GetTime()/1000));


	glPopMatrix();

	// Muda para modo de projeção perspectiva 3D
	PerspectiveMode();

	return true;
}




void CScene1::MouseMove(void) // Tratamento de movimento do mouse
{
	// Realiza os cálculos de rotação da visão do Player (através das coordenadas
	// X do mouse.
	POINT mousePos;
	int middleX = WIDTH >> 1;
	int middleY = HEIGHT >> 1;

	GetCursorPos(&mousePos);

	if ((mousePos.x == middleX) && (mousePos.y == middleY)) return;

	SetCursorPos(middleX, middleY);

	fDeltaX = (float)((middleX - mousePos.x)) / 10;
	fDeltaY = (float)((middleY - mousePos.y)) / 10;

	// Rotaciona apenas a câmera
	pCamera->rotateGlob(-fDeltaX, 0.0f, 1.0f, 0.0f);
	pCamera->rotateLoc(-fDeltaY, 1.0f, 0.0f, 0.0f);
}

void CScene1::KeyPressed(void) // Tratamento de teclas pressionadas
{

	// Verifica se a tecla 'W' foi pressionada e move o Player para frente
	if (GetKeyState('W') & 0x80)
	{
		pCamera->moveGlob(pCamera->Forward[0], pCamera->Forward[1], pCamera->Forward[2]);
	}
	// Verifica se a tecla 'S' foi pressionada e move o Player para tras
	else if (GetKeyState('S') & 0x80)
	{
		pCamera->moveGlob(-pCamera->Forward[0], -pCamera->Forward[1], -pCamera->Forward[2]);
	}
	// Verifica se a tecla 'A' foi pressionada e move o Player para esquerda
	else if (GetKeyState('A') & 0x80)
	{
		pCamera->moveGlob(-pCamera->Right[0], -pCamera->Right[1], -pCamera->Right[2]);
	}
	// Verifica se a tecla 'D' foi pressionada e move o Player para direira
	else if (GetKeyState('D') & 0x80)
	{
		pCamera->moveGlob(pCamera->Right[0], pCamera->Right[1], pCamera->Right[2]);
	}
	// Senão, interrompe movimento do Player
	else
	{
	}


	if (GetKeyState(VK_UP) & 0x80)
	{
		fPosZ -= fMovementFactor;
	}
	if (GetKeyState(VK_DOWN) & 0x80)
	{
		fPosZ += fMovementFactor;
	}
	if (GetKeyState(VK_LEFT) & 0x80)
	{
		fPosX -= fMovementFactor;
	}
	if (GetKeyState(VK_RIGHT) & 0x80)
	{
		fPosX += fMovementFactor;
	}
	if (GetKeyState(VK_PRIOR) & 0x80)
	{
		fPosY += fMovementFactor;
	}
	if (GetKeyState(VK_NEXT) & 0x80)
	{
		fPosY -= fMovementFactor;
	}
}

void CScene1::KeyDownPressed(WPARAM	wParam) // Tratamento de teclas pressionadas
{
	switch (wParam)
	{
	case VK_TAB:
		bIsWireframe = !bIsWireframe;
		break;

	case VK_SPACE:
	{
		pTimer->Init();
	}
		break;

	case VK_RETURN:

		break;

	case 'X':
	{
		fRotX = 1.0f;
		fRotY = 0.0f;
		fRotZ = 0.0f;
	}
		break;

	case 'Y':
	{
		fRotX = 0.0f;
		fRotY = 1.0f;
		fRotZ = 0.0f;
	}
		break;

	case 'Z':
	{
		fRotX = 0.0f;
		fRotY = 0.0f;
		fRotZ = 1.0f;
	}
		break;

	}

}

//	Cria um grid horizontal ao longo dos eixos X e Z
void CScene1::Draw3DSGrid(float width, float length)
{

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.0f, 0.3f, 0.0f);
	glPushMatrix();
	for (float i = -width; i <= length; i += 1)
	{
		for (float j = -width; j <= length; j += 1)
		{
			// inicia o desenho das linhas
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(i, 0.0f, j + 1);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(i + 1, 0.0f, j + 1);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(i + 1, 0.0f, j);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(i, 0.0f, j);
			glEnd();
		}
	}
	glPopMatrix();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


//void CScene1::DrawCube(float pX, float pY, float pZ,
//	float rX, float rY, float rZ, float angle,
//	float sX, float sY, float sZ,
//	int texID)
//{
//
//	// Seta a textura ativa
//	if (texID >= 0)
//		pTextures->ApplyTexture(texID);
//
//	glPushMatrix();
//	glTranslatef(pX, pY, pZ);
//	glRotatef(angle, rX, rY, rZ);
//	glScalef(sX, sY, sZ);
//
//	glBegin(GL_QUADS);
//	// face frente
//	glTexCoord2d(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
//	glTexCoord2d(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, 0.5f);
//	glTexCoord2d(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, 0.5f);
//	glTexCoord2d(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, 0.5f);
//
//	// face trás
//	glTexCoord2d(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
//	glTexCoord2d(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
//	glTexCoord2d(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
//	glTexCoord2d(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
//	
//	// face direita
//	glTexCoord2d(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
//	glTexCoord2d(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
//	glTexCoord2d(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
//	glTexCoord2d(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
//
//	// face esquerda
//	glTexCoord2d(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
//	glTexCoord2d(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
//	glTexCoord2d(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
//	glTexCoord2d(1.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
//
//	// face baixo
//	glTexCoord2d(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
//	glTexCoord2d(0.0f, 1.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
//	glTexCoord2d(1.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
//	glTexCoord2d(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
//
//	// face cima
//	glTexCoord2d(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
//	glTexCoord2d(0.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
//	glTexCoord2d(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
//	glTexCoord2d(1.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  -0.5f);
//
//	glEnd();
//
//	glPopMatrix();
//
//
//}

void CScene1::DrawAxis()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	// Eixo X
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1000.0f, 0.0f, 0.0f);
	glVertex3f(1000.0f, 0.0f, 0.0f);

	// Eixo Y
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1000.0f, 0.0f);
	glVertex3f(0.0f, -1000.0f, 0.0f);

	// Eixo Z
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1000.0f);
	glVertex3f(0.0f, 0.0f, -1000.0f);
	glEnd();
	glPopMatrix();
}

vector<string> CScene1::Split(string s, string delimiter) {
	size_t pos = 0;
	string token;
	vector<string> v;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		v.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	v.push_back(s);

	return v;
}

vector<Quadrado> CScene1::LerArquivo(string caminho) {

	vector<string> sVertices, sTexturas, sPoligonos;
	vector<Vertice> vVertices;
	vector<Textura> tTexturas;
	vector<Quadrado> qQuadrados;	

	/// Lendo o arquivo
	ifstream arquivo;
	string linha;
	arquivo.open(caminho);
	if (arquivo.is_open()) {
		while (getline(arquivo, linha)) {
			/// Armazenando os vértices
			if (linha[0] == 'v' && linha[1] == ' ') {
				sVertices = Split(linha.substr(3, linha.length()), " ");
				Vertice v;
				v.x = atof(sVertices[0].c_str());
				v.y = atof(sVertices[1].c_str());
				v.z = atof(sVertices[2].c_str());
				vVertices.push_back(v);
			}

			/// -> Armazenando as normais

			/// Armazenando as texturas
			if (linha[0] == 'v' && linha[1] == 't') {
				sTexturas = Split(linha.substr(3, linha.length()), " ");
				Textura t;
				t.x = atof(sTexturas[0].c_str());
				t.y = atof(sTexturas[1].c_str());
				t.z = atof(sTexturas[2].c_str());
				tTexturas.push_back(t);
			}

			/// Armazenando os poligonos e buscando as infos nos outros vectores
			if (linha[0] == 'f') {
				sPoligonos = Split(linha.substr(2, linha.length()), " ");
				sPoligonos.erase(sPoligonos.begin() + 4);

				Quadrado q;
				for (int i = 0; i < sPoligonos.size(); i++) {
					char sPosicaoVertice = sPoligonos[i][0];
					char sPosicaoTextura = sPoligonos[i][2];
					char sPosicaoNormal  = sPoligonos[i][4];
					Vertice v = vVertices[(sPosicaoVertice - '0') - 1];
					Textura t = tTexturas[(sPosicaoTextura - '0') - 1];
					switch (i) {
					case 0:
						q.vertice1 = v;
						q.textura1 = t;
						break;
					case 1:
						q.vertice2 = v;
						q.textura2 = t;
						break;
					case 2:
						q.vertice3 = v;
						q.textura3 = t;
						break;
					case 3:
						q.vertice4 = v;
						q.textura4 = t;
						break;
					}
				}
				qQuadrados.push_back(q);
			}
		}
	}
	else {
		cout << "Falha ao abrir o arquivo";
	}

	return qQuadrados;
}

//void CScene1::CreateSkyBox(float x, float y, float z, float width, float height, float length)
//{
//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//	glPushMatrix();
//
//	// Centraliza o Skybox em torno da posição especificada(x, y, z)
//	x = x - width / 2;
//	y = y - height / 2;
//	z = z - length / 2;
//
//
//	// Aplica a textura que representa a parte da frente do skybox (BACK map)
//	pTextures->ApplyTexture(0);
//
//	// Desenha face BACK do cubo do skybox
//	glBegin(GL_QUADS);
//	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
//	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z);
//	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z);
//	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
//	glEnd();
//
//
//	// Aplica a textura que representa a parte da frente do skybox (FRONT map)
//	pTextures->ApplyTexture(1);
//
//	// Desenha face FRONT do cubo do skybox
//	glBegin(GL_QUADS);
//	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
//	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
//	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
//	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);
//	glEnd();
//
//
//	// Aplica a textura que representa a parte da frente do skybox (DOWN map)
//	pTextures->ApplyTexture(2);
//
//	// Desenha face BOTTOM do cubo do skybox
//	glBegin(GL_QUADS);
//	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
//	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z + length);
//	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z + length);
//	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
//	glEnd();
//
//
//	// Aplica a textura que representa a parte da frente do skybox (UP map)
//	pTextures->ApplyTexture(3);
//
//	// Desenha face TOP do cubo do skybox
//	glBegin(GL_QUADS);
//	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
//	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
//	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
//	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z + length);
//	glEnd();
//
//
//	// Aplica a textura que representa a parte da frente do skybox (LEFT map)
//	pTextures->ApplyTexture(4);
//
//	// Desenha face LEFT do cubo do skybox
//	glBegin(GL_QUADS);
//	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
//	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
//	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
//	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
//	glEnd();
//
//
//	// Aplica a textura que representa a parte da frente do skybox (RIGHT map)
//	pTextures->ApplyTexture(5);
//
//	// Desenha face RIGHT do cubo do skybox
//	glBegin(GL_QUADS);
//	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
//	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z + length);
//	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
//	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
//	glEnd();
//
//	glPopMatrix();
//}
