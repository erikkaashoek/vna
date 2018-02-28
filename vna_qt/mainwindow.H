#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <xavna/common.H>
namespace Ui {
class MainWindow;
}
class QPushButton;
class PolarView;
class ImpedanceDisplay;

namespace QtCharts {
class QChartView;
class QChart;
class QValueAxis;
}
namespace xaxaxa {
class VNADevice;
class VNACalibration;
}
class QTimer;
class MarkerSlider;

using namespace QtCharts;
using namespace xaxaxa;
using namespace std;

struct SParamViewSource {
    int row,col;    // which S parameter are we viewing
    enum Types {
        UNDEFINED=0,
        TYPE_MAG=1,     // view must be QLineSeries
        TYPE_PHASE,     // view must be QLineSeries
        TYPE_GRPDELAY,  // view must be QLineSeries
        TYPE_COMPLEX,    // view must be PolarView
        _LAST
    } type;
};

// a UI view of one S parameter vs frequency
struct SParamView {
    SParamViewSource src;
    QObject* view;

    // these are not applicable for PolarView
    vector<QObject*> markerViews; // a QScatterSeries for each marker if view is a QLineSeries
    function<void(SParamView&)> addMarker;   // this function must add a new object to markerViews
};
struct Marker {
    int freqIndex;
    MarkerSlider* ms;
    bool enabled;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const char* calButtonDoneStyle = "background-color: #9999ff";

    VNADevice* vna=NULL;
    ImpedanceDisplay* impdisp=NULL;
    QTimer* timer=NULL;
    PolarView* polarView=NULL;

    // array of values directly from the vna, updated every time a frequency point arrives
    vector<VNARawValue> rawValues;
    // same as rawValues, but with calibration applied (if any)
    vector<VNACalibratedValue> values;
    // array of UI views of the data; each view has a source (describing what data to display) and a widget
    // (where to display the data on
    vector<SParamView> views;
    // array of axis to update when frequency sweep params are changed
    vector<QValueAxis*> xAxis;
    // user-added markers
    vector<Marker> markers;
    // for each reference standard, map from name to the measure button
    map<string, QWidget*> calButtons;
    // for each reference standard, map from name to measured values
    map<string, vector<VNARawValue> > calMeasurements;
    // current active calibration
    const VNACalibration* curCal = NULL;
    // current calibration coefficients
    vector<MatrixXcd> curCalCoeffs;

    double freqScale = 1e-6;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void populateCalTypes();
    // setup the line graph and smith chart widgets, and populate this->views
    void setupViews();
    // set the callbacks of this->vna
    void setCallbacks();

    // call after changing frequency sweep parameters of vna
    void updateSweepParams();
    // update the impedance displays
    void updateValueDisplays();
    void updateMarkerViews(int marker=-1);
    void updateBottomLabels(int marker=-1);
    void addMarker(bool removable);
    void enableUI(bool enable);


    string freqStr(double freqHz);

protected:

private slots:
    void on_d_caltype_currentIndexChanged(int index);
    void btn_measure_click(QPushButton* btn);
    void on_actionOther_triggered();
    void on_slider_valueChanged(int value);
    void calMeasurementCompleted(string calName);

    void on_b_clear_m_clicked();

    void on_b_apply_clicked();

    void on_b_clear_clicked();

    void on_actionSweep_params_triggered();

    // update a single point on all views in this->views
    void updateViews(int freqIndex);

    // update a single point on a view, or all points on a view (if freqIndex is -1)
    void updateView(int viewIndex, int freqIndex=-1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H