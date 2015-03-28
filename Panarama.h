/*
 * This file is part of ExpertSDR
 *
 * ExpertSDR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * ExpertSDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Copyright (C) 2012 Valery Mikhaylovsky
 * The authors can be reached by email at maksimus1210@gmail.com
 */

#ifndef PANARAMA_H
#define PANARAMA_H

#include <search.h>
#include <QtGui>
#include <QThread>
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLShader>
#include <GL/glu.h>
#include "PanaramDefines.h"
#include "PanoramOpt.h"
#include "DttSP.h"
#include "Timer/Timer.h"
#include "GLFont/glfont.h"

#define SIZE 512
#define RULE_HEIGH 20.0f
#define TX_TO_RX_TIME 20
#define DISTANCE -10.0
#define APPOROACH 0.0
#define BUFF_SIZE 4096
#define CIRCLE_BUFF_SIZE 20
#define PAN_UPDATE 25
#define IMAGE_LEN 512
#define IMAGE_SUB_HEIGHT 1
#define NUM_COLOR 4
#define MIN_PX_DB 10
#define NUM_MEANE_BUFF CIRCLE_BUFF_SIZE

#define FREQ_OFFSET_LIMIT 500000




 #ifndef GL_MULTISAMPLE
 #define GL_MULTISAMPLE  0x809D
 #endif

 #ifndef GL_VERSION_1_2
 #define GL_TEXTURE_3D 0x806F
 #define GL_TEXTURE_WRAP_R 0x8072
 #define GL_CLAMP_TO_EDGE 0x812F
 #define GL_BGRA 0x80E1
 #endif

 #ifndef GL_VERSION_1_3
 #define GL_TEXTURE0 0x84C0
 #define GL_TEXTURE1 0x84C1
 #define GL_TEXTURE2 0x84C2
 #define GL_TEXTURE_CUBE_MAP 0x8513
 #define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
 #endif

 #ifndef GL_VERSION_1_5
 #define GL_ARRAY_BUFFER 0x8892
 #define GL_ELEMENT_ARRAY_BUFFER 0x8893
 #define GL_READ_WRITE 0x88BA
 #define GL_STATIC_DRAW 0x88E4
 #endif

 #ifndef GL_EXT_framebuffer_object
 #define GL_RENDERBUFFER_EXT 0x8D41
 #define GL_FRAMEBUFFER_EXT 0x8D40
 #define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
 #define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
 #define GL_DEPTH_ATTACHMENT_EXT 0x8D00
 #endif

typedef struct
{
	float offset;
	float scale;
}DBM_TRX_STATE;

class Panarama : public QGLWidget
{
    Q_OBJECT

    Timer gridTimer;
    int autoStepFreq;
    int freqStart, freqEnd;
    double timeStart, timeEnd;
    DBM_TRX_STATE DbmRxState[NUM_BANDS+1];
    DBM_TRX_STATE DbmTxState;
    int currentBand;
    int TimerId;
    OBJECTS Object;
    OBJECTS LastPosObject;
    QCursor cursor;
    QPoint LastPos;
    QPoint ReleasePos;
    RANGE RangeFreq;
    CURSOR Cursor;
    TRXMODE TRxMode, TRxModeOld;
    bool isUpdate;
    bool IsWindow;
    bool IsCrossF1;
	GLFont *info_font;
	GLFont *dbm_font;
	GLFont *band_font;
	GLFont *cursor_font;
    QPixmap *pRulePixmap;
    GLuint RuleTexture;
    QPixmap *pLeftRange;
    GLuint LeftRange;
    QPixmap *pRightRange;
    GLuint RightRange;
    QPixmap *pBackgroundPixmap;
    QPixmap *pBackgroundPixmap2;
    QPixmap *pBackgroundPixmap3;
    QPixmap *pBackgroundPixmap4;
    GLuint BackgroundTexture;
    GLuint BackgroundTexture2;
    GLuint BackgroundTexture3;
    GLuint BackgroundTexture4;
    QPixmap *pGlassPixmap;
    GLuint GlassTexture;
    double HeighRule;
    GLdouble PresPosX;
    GLdouble PresPosY;
    GLdouble MovePosX;
    GLdouble MovePosY;
    GLdouble PresPosX2;
    GLdouble PresPosY2;
    GLdouble MovePosX2;
    GLdouble MovePosY2;
    int sPosRule, dPosRule;
    GLfloat posRule;
    GLfloat lenRule;
    GLdouble sRuleYPos, dRuleYPos;
    GLdouble sRuleYPosTx, dRuleYPosTx;
    GLdouble ScaleWindowX;
    GLdouble ScaleWindowY;
    BACKGROUND BackgroundType;
    int sFreqDDS, dFreqDDS;
    int sFreqFilter1, dFreqFilter1;
    int numStepGridDDS;
    int numStepGridFilter;
    int SampleRate;
    GLdouble sDDSFreq, dDDSFreq;
    GLdouble sFilter, dFilter;
    GLdouble sBandPassLow, dBandPassLow;
    GLdouble sBandPassHigh, dBandPassHigh;
    GLdouble sPich, dPich;
    GLdouble sPich2, dPich2;
    int BandLow, BandHigh;
    int BandLow2, BandHigh2;
    int BandTxLow, BandTxHigh;

    bool IsFilter2;
    GLdouble sFilter2, dFilter2;
    GLdouble sBandPassLow2, dBandPassLow2;
    GLdouble sBandPassHigh2, dBandPassHigh2;
    QColor ColorFilter2;
    GLdouble sPosZoomPan, dPosZoomPan;
    GLdouble LimitMinDDSFreq;
    GLdouble LimitMaxDDSFreq;
    GLdouble LimitMinLowBand;
    GLdouble LimitMaxHighBand;
    GLdouble oy1;
    GLdouble oy2_5;
    GLdouble oy5;
    GLdouble oy10;
    GLdouble oy20;
    GLdouble oy30;
    GLdouble ox1;
    GLdouble ox2_5;
    GLdouble ox5;
    GLdouble ox10;
    GLdouble ox20;
    GLdouble ox30;
    float *pBuff;
    float **pCircleBuff;
    float MedianTimeBuff[7][BUFF_SIZE];
    float *BufWf;
    float *pSpBuff;
    int SizeSpBuff;
    int MeanBuff;
    int CntUpdateBuff;
    int CircleCnt;

    QTimer TimerMeanBuff;
    QTimer TimerDrawFilterEdges;
    QTimer TimerDrawFilterEdges2;
    QTimer TimerDbmPos;
    QTimer TimerFilterPos;
    QTimer TimerFilter2Pos;
    QTimer TimerPichPos;
    QTimer TimerPichPos2;
    QTimer TimerAutosetDbm;
    QTimer TimerPeriodAutosetDbm;
    QTimer TimerPeriodGetMax;
    QTimer TimerTxToRx;

    bool IsPeriodGetMax;
    float minDbm;
    float maxDbm;
    bool IsChangedDbmLevel;
    GLdouble ScaleXPanoram;
    GLdouble sScaleRuleX, dScaleRuleX;
    GRID_SCALE_TYPE GridScaleType;
    GLdouble ValLenStep;

    GLdouble BeginX;
    GLdouble EndX;
    int StepCount;
	GLdouble Len;
	GLdouble Step;

	GLfloat LineSpBuff[BUFF_SIZE][2];
	QColor ColorTop, ColorBot, ColorLine, ColorSp, ColorFilter;
	GLfloat ArrayV[BUFF_SIZE*3];
	GLfloat ArrayV2[BUFF_SIZE*3];
	GLfloat ColorV[BUFF_SIZE*4];

	GLdouble BeginGrid;
	GLdouble GridStep;
	int CountGridStep;
	int CntStepGrid;
	double StepNum;
	int PrecisionNum;
	GLdouble OffsetNum;

	QPoint LastPosPress;
	QPoint LastPosRelease;

	double LowRange;
	double HighRange;

	bool IsDrawFilterEdges;
	bool IsPressFilterEdges;
	bool IsDrawFilterEdges2;
	bool IsPressFilterEdges2;
	int AlphaColor;
	int AlphaColor2;
	int AlphaColorPich;
	int AlphaColorPich2;
	GLdouble AlphaColorCF;
	GLdouble AlphaColorCF2;

	GLdouble sOffsetDbm, dOffsetDbm;
	GLdouble AlphaColorDbm;
	GLdouble sLenDbmY, dLenDbmY;
	GLdouble StepDbm;
	int stepDb;
	int LowDb;
	int HighDb;
	GLfloat PosYNum, PosYNumTx;
	double offsetDbm, scaleDbm;

	float level_min;
	float level_max;
	int level_range;
	float level_multiplier;
	static const int levels_count = 256 * 6;
	int last_update;
	int waterfall_mode;

	GLdouble SaveOffsetDbm;
	GLdouble SaveLenDbmY;

	int IndxSubTexture;
	int NumLineTextures;
	int CntLinesTexture;
	int CntLineSpeed;
	double LineSpeedMax;

	static const int WF_TEX_W_CNT = 8;
	static const int WF_TEX_H_CNT = 3;

	GLubyte	pNewLineImage[WF_TEX_W_CNT * IMAGE_LEN][NUM_COLOR];
	GLuint WFTextures[WF_TEX_H_CNT][WF_TEX_W_CNT];

	ACTION_OBJECT ActionObject;

    GLfloat Vis;
    GLuint Texture;
    GLfloat rotationX;
    GLfloat rotationY;
    GLfloat rotationZ;
    QPoint lastPos;

    bool IsVisibleInfo;
    double SignalDds;
    double SignalFilter;
    double SignalBandLow;
    double SignalBandHigh;
    double SignalFilter2;
    double SignalBandLow2;
    double SignalBandHigh2;

    QTimer TimerSignalDds;
    QTimer TimerSignalFilter;
    QTimer TimerSignalBandLow;
    QTimer TimerSignalBandHigh;
    QTimer TimerChangedFilter;

    PAN_OPTION PanOption;
    float CalibrateLevel;
    int CntMeaneBuff;
    int CntBegin;
    float MeanMin[NUM_MEANE_BUFF];
    float MeanMax[NUM_MEANE_BUFF];
    bool IsStart;
    bool IsGridChanged;
    bool IsMoveGrid;
    bool IsDamping;
    double StepMoveGrid;
    double StepDamping;
    QPoint FirstPos;
    QPoint SecondPos;
    QTimer TimerMousePos;
    int CntLenMove;
    int pitchVal;
	int TxVfo;
    int FreqVfoB;
    SDRMODE Mode;
    int TrxCnt;
    int bandNum;
    bool isOutOfRit;
    bool isRitOn;
    int ritValue;
    int signalRitValue;
    int sRitValue, dRitValue;
    float sRit, dRit;
    int ritLimLow, ritLimHigh;
    float glRitLimLow, glRitLimHigh;
    int currentFilter1Freq;
    float alphaRitLim;
    QTimer timerAlphaColor;
    int fps;

public:
    PanoramOpt *pPanOpt;

    Panarama(QWidget *pOpt,QWidget *parent = 0);
    ~Panarama();

    void readSettings(QSettings *pSettings);
    void writeSettings(QSettings *pSettings);

    void SetSpectrumBuffer(float *pBuffer, int Size);
    void SetSampleRate(int Freq);
    int  GetSampleRate();
    void SetDDSFreq(int Freq);
    int  GetDDSFreq();
    void GetBandFilter(double &Low, double &High);
    void SetBandFilter(int Low, int High);
    void SetFilter(int Freq);
    int  GetFilter();
    void SetPitch(int Freq);
    int  GetPitch();
    void SetCalibrateK(float Val);
    float GetfreqForMaxLevel();
    void SetTxBandFilter(int Low, int High);
    void SetBand(int band);
    void SetLowBand(int Freq);
    void SetHighBand(int freq);
    void SetLockScreen(bool state);
    void SetLockBandFilter(bool state);
    void SetLockFilter(bool state);
    void SetLockGrid(bool ssate);
    void SetTRxMode(TRXMODE Mode);
	TRXMODE GetTrxMode();
    void Start();
    void Stop();
    void SetScaleGrid(float Value);
    float GetScaleGrid();
    void Filter2On(bool state);
    bool IsFilter2On();
    void SetFilter2(int Freq);
    int GetFilter2();
    void GetBandFilter2(double &Low, double &High);
    void SetBandFilter2(int Low, int High);
    bool IsFilter2OnPanoram();
    void SetMode(SDRMODE mode);
    void GetPeak(int *freq, float *dbm);

    bool IsMoveUpdate;
    bool LockGrid;
    bool LockScreen;
    bool LockBand;
    bool LockFilter;
    bool IsChangedFilter;
    bool IsEnableSp;
    int MeanVal;
    QPoint posPress, posMove;
    int getFps() {return fps;};
    bool IsXvrt;

public slots:
	void SetVisibleInfo(int state);
    void SetSpectrRate(int Val);
    void SetWaterfallRate(int Val);
    void SpectrumEnable(bool State);
	void SetSetupFilterMaxLevel();
    void SetTxVfo(int val);
    void SetStepFilter(int index);
    void SetStepDDS(int index);
    void setRit(bool state);
    bool isRit();
    void setRitValue(int value);
    int getRitValue();

private:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void MedianFilter(float *pIn, int size);
    int faceAtPosition(const QPoint &pos);
    void DrawBaseElement();
    void DrawBackground();
    void DrawSpectr();
    void DrawRule();
    void DrawGrid();
    void DrawFilter();
    void DrawCursor();
    void DrawDbm();
    void DrawWaterfall();
    void DrawZoomer();
    void DrawInfo(GLdouble X, GLdouble Y, QString Str, QColor color);
    void SetStepGrid();
    void SetStepDbm();
    void UpdateLevels();
    void GetRGB(float Value, GLubyte *red, GLubyte *green, GLubyte *blue);
    void MakeSubTexture();
    float GetMin(float *pBuff, int Size);
    float GetMax(float *pBuff, int Size);
    float GetMean(float *pBuff, int Size);
    int getBandNum();
    void updateDbmState();
    QString freqToStr(double freq);
	void draw_filter_rect(int freq, int low, int high, QColor color, bool draw_edge, QColor edge_color, GLdouble z_pos);
	void draw_filter_center(int freq, QColor color, QColor higlight_color, GLdouble z_pos);
	void draw_filter_info(int vfo, int freq, int low, int high, bool show_rxtx_mark, bool is_tx, bool visible_filter_info, bool draw_on_left_side, QString vfo_name);

private slots:
	void MeanTimer();
	void FilterEdges();
	void FilterEdges2();
	void DbmPos();
	void FilterPos();
	void Filter2Pos();
	void PichPos();
	void PichPos2();
	void AutosetDbm();
	void SetPowerComp(int Index);
	void ChangedAlphaColorSp(int Val);
	void SetTypeAutomatic(int Type);
    void SlotChangedParametersFreq();
    void SlotChangedSettings();
    void SlotChangeFilter();
    void SlotPeriodGetMax();
    void SlotTxToRx();
    void SlotMousePos();
    void SlotRitColor();

signals:
	void ChangeCentrFreq(int Freq);
	void ChangeFilterFreq(int Freq);
	void ChangeFilterHighFreq(int Freq);
	void ChangeFilterLowFreq(int Freq);
	void ChangeFilterFreq2(int Freq);
	void ChangeFilterHighFreq2(int Freq);
	void ChangeFilterLowFreq2(int Freq);
	void ChangeStepDDS(int val);
	void ChangeZoom(double val);
	void ChangeZoomPos(double val);
	void IsOnFilter2(bool);
    void ChangedDbm(double offsetDb, double scalseDb);
    void ChangeRitValue(int Freq);

    void changedFps(int);
};

#endif // PANARAMA_H
