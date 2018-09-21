#include "widget.h"

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	//QPushButton *pushButton1 = new QPushButton;
	//pushButton1->setObjectName("pushButton1");
	//QPushButton *pushButton2 = new QPushButton;
	//pushButton1->setObjectName("pushButton2");
	//QPushButton *pushButton3 = new QPushButton;
	//pushButton1->setObjectName("pushButton3");

	ui.setupUi(this);

	//connect(pushButton1, &QPushButton::clicked, this, &Widget::ChangeSinOffset);
	//connect(pushButton2, &QPushButton::clicked, this, &Widget::ChangeAmplitude);
	//connect(pushButton3, &QPushButton::clicked, this, &Widget::ChangeCycle);	

}

Widget::~Widget()
{

}

/**************************************************************************
Function Name:   InitSinGrid
Description:     ��ʼ������
**************************************************************************/
void Widget::InitSinGrid()
{

	m_FHRQwtPlot = new QwtPlot(this);
	m_FHRQwtPlot->move(10, 5);
	m_FHRQwtPlot->setFixedWidth(780);    // ����������Ĵ�С
	m_FHRQwtPlot->setFixedHeight(210);
	m_FHRQwtPlot->setTitle(" ");         // ����������ı���
	m_FHRQwtPlot->setAxisScale(QwtPlot::yLeft, 30, 240, 30); // ������ķ�Χ�ͼ��
	m_FHRQwtPlot->setAxisScale(QwtPlot::yRight, 30, 240, 30);
	m_FHRQwtPlot->setAxisScale(QwtPlot::xBottom, 0, 100, 20);
	m_FHRQwtPlot->enableAxis(QwtPlot::yRight, true);
	m_FHRQwtPlot->enableAxis(QwtPlot::xBottom, false);  // ����X��
	m_FHRQwtPlot->setStyleSheet("background:transparent;");//ɾ�������ᱳ��
	m_FHRQwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 4);
	m_FHRQwtPlot->setAxisMaxMinor(QwtPlot::yLeft, 3);//����ÿһ������ж��ٸ�С��
	m_FHRQwtPlot->setAxisMaxMinor(QwtPlot::yRight, 3);
	m_FHRQwtPlot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(QDateTime::currentDateTime()));// ��������ʾ��ǰʱ��(��Ҫ��X����ʾ������������ʾ��ʱ��)
	m_FHRQwtPlot->setCanvasBackground(Qt::white); // ���ñ�����ɫ

												  // ɾ���������ļ��
	for (int n = 0; n < m_FHRQwtPlot->axisCnt; n++)
	{
		QwtScaleWidget *poScaleWidget = m_FHRQwtPlot->axisWidget(n);
		if (poScaleWidget)
		{
			poScaleWidget->setMargin(0);
		}

		QwtScaleDraw *poScaleDraw = m_FHRQwtPlot->axisScaleDraw(n);
		if (poScaleDraw)
		{
			poScaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
		}
	}
	m_FHRQwtPlot->plotLayout()->setAlignCanvasToScales(true);

	//������
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->enableX(true);
	grid->enableY(true);
	grid->setMajorPen(QPen(Qt::gray, 1, Qt::SolidLine));  // ������
	grid->setMinorPen(QPen(QColor(186, 186, 186), 0, Qt::DotLine));// С����
	grid->attach(m_FHRQwtPlot);

	// ������ϵ�������
	m_FHRCurve = new QwtPlotCurve;
	m_FHRCurve->setPen(QColor(255, 85, 255), 2, Qt::SolidLine);
	m_FHRCurve->setCurveAttribute(QwtPlotCurve::Fitted, true);   //Բ����ʾ����
	m_FHRCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	m_FHRCurve->attach(m_FHRQwtPlot);  // �����߼��ص���������

									   //m_UpdateFlag = 0;

									   //m_LimitBackGroud = new Background(m_HighLimit,m_LowLimit);//����ͼ��䱳��
									   //m_LimitBackGroud->attach( m_FHRQwtPlot );//����plot
									   // ������ʱ��
	m_SinTimer = new QTimer();
	connect(m_SinTimer, SIGNAL(timeout()), this, SLOT(TimeSlot()));
	StartSin();   // ��ʼ�����̣߳�����Sin�����(����SinData.h����Ľӿں���)
	m_SinTimer->start(500);//0.5�봥����ʱ��

}



/**************************************************************************
Function Name:   TimeSlot
Description:     ��ʱ���ۺ�������������Sinͼ��
**************************************************************************/
void Widget::TimeSlot()
{
	static int s_BeginAixsX = 0;  // ��ʼ������
	static int s_Result;

	if ((s_Result = GetSinData(&m_SinPointX, &m_SinPointY)) <= 0)  // ��ȡSin�����
	{
		return;// ��ȡʧ�ܣ�ֱ�ӷ���
	}

	if (m_SinPointX >= 100)  // ����ǰ�����ử���ˣ��Ͱ���������ǰ�ƶ���(����X�᷶Χ)
	{
		s_BeginAixsX++;
		m_FHRQwtPlot->setAxisScale(QwtPlot::xBottom, s_BeginAixsX, m_SinPointX, 20);// ����������
		m_DataVectorX.erase(m_DataVectorX.begin(), m_DataVectorX.begin() + 1);
		m_DataVectorY.erase(m_DataVectorY.begin(), m_DataVectorY.begin() + 1);
	}

	m_DataVectorX.append((double)m_SinPointX);
	m_DataVectorY.append((double)m_SinPointY);
	m_FHRCurve->setSamples(m_DataVectorX, m_DataVectorY);  // ��ͼ

	m_FHRQwtPlot->replot();  // �ػ棬һ��Ҫ���÷���ûЧ��
}

//����sinͼ���Y��ƫ��������������ڵĲۺ�����

void Widget::ChangeSinOffset()
{
	m_UpdateFlag = DATA_UPDATA;
	static int s_SinOffset = 120;
	if (s_SinOffset == 160)
		s_SinOffset = 120;
	s_SinOffset += 10;
	SetSinOffset(s_SinOffset);
	UpdateData();
}

void Widget::ChangeAmplitude()
{

	static int s_SinAmplitude = 20;
	if (s_SinAmplitude == 50)
		s_SinAmplitude = 20;
	m_UpdateFlag = DATA_UPDATA;

	s_SinAmplitude += 5;
	SetAmplitude(s_SinAmplitude);
	UpdateData();

}

void Widget::ChangeCycle()
{
	static int s_SinCycle = 4;
	if (s_SinCycle == 10)
		s_SinCycle = 4;
	m_UpdateFlag = DATA_UPDATA;

	s_SinCycle += 1;
	SetCycle(s_SinCycle);
	UpdateData();
}

/**************************************************************************/
//Function Name:     updateData
//Description:     ����sinͼ�ε����꣬�ػ�
/**************************************************************************/
void Widget::UpdateData()
{
	//int axisX[m_SinPointX];
	//int axisY[m_SinPointX];
	int *axisX = new int[m_SinPointX];
	int *axisY = new int[m_SinPointX];
	UpdateSinData(axisX, axisY);  // SinData.h�Ľӿں���
	m_DataVectorX.clear();
	m_DataVectorY.clear();
	for (int n = 0; n <m_SinPointX; n++)
	{
		m_DataVectorX.append((double)axisX[n]);
		m_DataVectorY.append((double)axisY[n]);
	}

	if (m_SinPointX >100)
	{
		int len = m_DataVectorX.length() - 100;
		if (len < 0)
			len = 0 - len;
		//m_FHRQwtPlot->setAxisScale(QwtPlot::xBottom,len,m_SinPointX,20);
		m_DataVectorX.erase(m_DataVectorX.begin(), m_DataVectorX.begin() + len);
		m_DataVectorY.erase(m_DataVectorY.begin(), m_DataVectorY.begin() + len);
	}

	m_FHRCurve->setSamples(m_DataVectorX, m_DataVectorY);
	m_FHRQwtPlot->replot();
	m_UpdateFlag = 0;
}