#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QWidget>

namespace Ui {
class FormSettings;
}
//форма с настройками отображения
class FormSettings : public QWidget
{
    Q_OBJECT
public:
    explicit FormSettings(QWidget *parent = 0);
    ~FormSettings();
signals:
    void fog(bool);

    void grid(bool);
    void sky(bool);
    void vertical(bool);
    void ils(bool);
    void info(bool);
    void loadDataFlight();
    void loadLand();
    void terra(bool);
    void freq(int);
    void multTime(double);
    void densityFog(double);
    void sizeScene(double);
    void offsetMapX(double);
    void offsetMapZ(double);
    void rotateMap(double);
    //! загрузка рельефа из интернета: широта, долгота (град), размер области (км)
    void loadTerrainOnline(double lat, double lon, double sizeKm);
    //! подгрузка рельефа по ходу полета: включена, широта и долгота начала координат
    void terrainStreaming(bool enabled, double lat, double lon);
    //! сеть: сглаживание движения, удаление молчащих объектов, запись, воспроизведение
    void smoothing(bool);
    void staleTimeout(int seconds);
    void recording(bool);
    void playRecording();
public slots:
    //! состояние загрузки рельефа
    void setTerrainStatus(const QString &text);
    //! состояние записи/воспроизведения
    void setNetStatus(const QString &text);
    void setRecordingChecked(bool value);
    //! отметка подгрузки рельефа (при включении из командной строки)
    void setTerrainStreamingChecked(bool value, double lat, double lon);
private:
    Ui::FormSettings *ui;
};

#endif // FORMSETTINGS_H
