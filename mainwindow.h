#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSlider>
#include <QDir>
#include <QDebug>
#include <string>
#include <algorithm>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif

using namespace std;

struct music {
    string name;
    string path;
    music* next;
    music* prev;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void exitApp();
private slots:
    // Playback
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnNext_clicked();
    void on_btnPrev_clicked();

    // Playlist management
    void on_btnAddBegin_clicked();
    void on_btnAddEnd_clicked();
    void on_btnDelete_clicked();
    void on_btnDeleteEnd_clicked();
    void on_btnSearch_clicked();
    void on_btnShowPlaylist_clicked();

    // Media signals
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void on_progressSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *audioOutput;
#endif

    music *head;
    music *tail;
    music *current;
    bool isPlaying;
    bool isSliderPressed;

    // DSA operations
    void addMusicAtBegin(const string& name, const string& path);
    void addMusicAtEnd(const string& name, const string& path);
    void deleteMusicByName(const string& name);
    void deleteMusicAtEnd();
    music* searchMusicByName(const string& name);
    void displayPlaylist();
    void clearPlaylist();

    // Helpers
    string extractFileName(const string& path);
    void playCurrentSong();
    void updateUIWithCurrentSong();
};

#endif // MAINWINDOW_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QListWidgetItem>
#include <QString>
#include <string>

#ifdef Q_OS_WIN
#include <windows.h>
#include <mmsystem.h>
#endif

typedef struct music
{
    std::string name;
    std::string path;
    struct music* next;
    struct music* prev;
} music;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Doubly linked list operations
    void addAtEnd(const std::string &name, const std::string &path);
    void addAtBegin(const std::string &name, const std::string &path);
    bool deleteEnd();
    bool deleteSelected(const std::string &name);
    music* searchByName(const std::string &name);

    // WAV utilities
    qint64 getWavDurationMs(const QString &filepath); // returns -1 on error

    // Playback
    void playMusic(music* node);
    void stopMusic();
    void playNext();
    void playPrev();

private slots:
    // auto-connected slots from UI
    void on_pushButtonAddEnd_clicked();
    void on_pushButtonAddBegin_clicked();
    void on_pushButtonDeleteEnd_clicked();
    void on_pushButtonDeleteSelected_clicked();
    void on_pushButtonPlay_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonNext_clicked();
    void on_pushButtonPrev_clicked();
    void on_pushButtonSearch_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_sliderProgress_sliderMoved(int position);

    // timer tick to simulate progress
    void onProgressTimer();

private:
    Ui::MainWindow *ui;

    // Doubly linked list head/tail/current
    music* head;
    music* tail;
    music* current;

    // Playback simulation
    QTimer *progressTimer;
    qint64 durationMs;   // computed duration in ms for current song
    qint64 elapsedMs;    // how many ms we have been 'playing' (estimate)
    qint64 lastTickMs;   // QElapsed simulation not strictly necessary but we do basic increment

    // helpers
    void refreshListWidget();
    std::string getFileNameFromPath(const std::string &path);
    void clearList();

    // disable seeking warning: slider shows progress but seeking is not supported by PlaySound
    bool seekingSupported() const { return false; }
};

#endif // MAINWINDOW_H
