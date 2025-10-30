#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileInfo>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      head(nullptr), tail(nullptr), current(nullptr),
      isPlaying(false), isSliderPressed(false)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
#endif

    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::mediaStatusChanged);

    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::exitApp);
    connect(ui->progressSlider, &QSlider::sliderPressed, [this]() { isSliderPressed = true; });
    connect(ui->progressSlider, &QSlider::sliderReleased, [this]() { isSliderPressed = false; });

    ui->lblCurrentSong->setText("No song loaded");
    ui->progressSlider->setEnabled(false);
    ui->lblTime->setText("00:00");
    ui->lblDuration->setText("00:00");
}

MainWindow::~MainWindow() {
    clearPlaylist();
    player->stop();
    delete ui;
}

// ======================= DSA LIST OPERATIONS =======================
void MainWindow::addMusicAtBegin(const string& name, const string& path) {
    music *newNode = new music{name, path, head, nullptr};
    if (!head) { head = tail = current = newNode; }
    else { head->prev = newNode; head = newNode; }
    displayPlaylist();
}

void MainWindow::addMusicAtEnd(const string& name, const string& path) {
    music *newNode = new music{name, path, nullptr, tail};
    if (!tail) { head = tail = current = newNode; }
    else { tail->next = newNode; tail = newNode; }
    displayPlaylist();
}

void MainWindow::deleteMusicByName(const string& name) {
    music *temp = head;
    while (temp) {
        if (temp->name == name) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if (temp == current) { player->stop(); isPlaying = false; }
#endif
            if (temp->prev) temp->prev->next = temp->next; else head = temp->next;
            if (temp->next) temp->next->prev = temp->prev; else tail = temp->prev;
            delete temp; displayPlaylist(); return;
        }
        temp = temp->next;
    }
}

void MainWindow::deleteMusicAtEnd() {
    if (!tail) return;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (tail == current) { player->stop(); isPlaying = false; }
#endif
    music* temp = tail;
    tail = tail->prev;
    if (tail) tail->next = nullptr; else head = nullptr;
    delete temp; displayPlaylist();
}

music* MainWindow::searchMusicByName(const string& name) {
    music* temp = head;
    string search = name; transform(search.begin(), search.end(), search.begin(), ::tolower);
    while (temp) {
        string song = temp->name; transform(song.begin(), song.end(), song.begin(), ::tolower);
        if (song.find(search) != string::npos) return temp;
        temp = temp->next;
    }
    return nullptr;
}

void MainWindow::displayPlaylist() {
    QString text;
    music* temp = head; int count = 1;
    while(temp) {
        text += QString::number(count) + ". " + QString::fromStdString(temp->name);
        if(temp==current) text;
        text+="\n"; temp=temp->next; count++;
    }
    if(count==1) text="Playlist empty!";
    ui->textPlaylist->setPlainText(text);
}

void MainWindow::clearPlaylist() {
    music *temp = head;
    while(temp) { music* next = temp->next; delete temp; temp=next; }
    head = tail = current = nullptr;
}

// ======================= HELPERS =======================
string MainWindow::extractFileName(const string& path) {
    size_t pos = path.find_last_of("/\\"); return (pos != string::npos)? path.substr(pos+1) : path;
}

void MainWindow::playCurrentSong() {
    if(!current) return;
    player->setSource(QUrl::fromLocalFile(QString::fromStdString(current->path)));
    player->play();
    isPlaying = true;
    ui->progressSlider->setEnabled(true);
    updateUIWithCurrentSong();
}

void MainWindow::updateUIWithCurrentSong() {
    ui->lblCurrentSong->setText(current? "♫ "+QString::fromStdString(current->name)+" ♫" : "No song loaded");
}

// ======================= UI SLOTS =======================
void MainWindow::on_btnPlay_clicked() {
    if(!current) { if(head) current=head; else return; playCurrentSong(); return; }
    if(isPlaying) { player->pause(); isPlaying=false; ui->btnPlay->setText("Play"); }
    else { player->play(); isPlaying=true; ui->btnPlay->setText("Pause"); }
}

void MainWindow::on_btnStop_clicked() {
    player->stop(); isPlaying=false; ui->btnPlay->setText("Play");
}

void MainWindow::on_btnNext_clicked() {
    if(!current) return;
    if(current->next) current=current->next; else current=head;
    playCurrentSong();
}

void MainWindow::on_btnPrev_clicked() {
    if(!current) return;
    if(current->prev) current=current->prev; else current=tail;
    playCurrentSong();
}

void MainWindow::on_btnAddBegin_clicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Select Songs", QDir::homePath(),
                                                      "Audio Files (*.mp3 *.wav *.ogg)");
    for(QString f: files) addMusicAtBegin(f.toStdString().substr(f.lastIndexOf("/")+1), f.toStdString());
}

void MainWindow::on_btnAddEnd_clicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Select Songs", QDir::homePath(),
                                                      "Audio Files (*.mp3 *.wav *.ogg)");
    for(QString f: files) addMusicAtEnd(f.toStdString().substr(f.lastIndexOf("/")+1), f.toStdString());
}

void MainWindow::on_btnDelete_clicked() {
    bool ok; QString name = QInputDialog::getText(this, "Delete Song", "Enter song name:", QLineEdit::Normal, "", &ok);
    if(ok && !name.isEmpty()) deleteMusicByName(name.toStdString());
}

void MainWindow::on_btnDeleteEnd_clicked() { deleteMusicAtEnd(); }

void MainWindow::on_btnSearch_clicked() {
    bool ok; QString name = QInputDialog::getText(this, "Search Song", "Enter song name:", QLineEdit::Normal, "", &ok);
    if(ok && !name.isEmpty()) {
        music* res = searchMusicByName(name.toStdString());
        if(res) { current=res; playCurrentSong(); }
        else QMessageBox::information(this,"Search Result","Song not found!");
    }
}

void MainWindow::on_btnShowPlaylist_clicked() { displayPlaylist(); }

// ======================= MEDIA SIGNALS =======================
void MainWindow::positionChanged(qint64 position) {
    if(!isSliderPressed) ui->progressSlider->setValue(position/1000);
    QTime t(0,(position/60000)%60,(position/1000)%60);
    ui->lblTime->setText(t.toString("mm:ss"));
}

void MainWindow::durationChanged(qint64 duration) {
    ui->progressSlider->setMaximum(duration/1000);
    QTime t(0,(duration/60000)%60,(duration/1000)%60);
    ui->lblDuration->setText(t.toString("mm:ss"));
}

void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if(status==QMediaPlayer::EndOfMedia) on_btnNext_clicked();
}

void MainWindow::on_progressSlider_sliderMoved(int position) {
    player->setPosition(position*1000);
}
void MainWindow::exitApp() {
    QApplication::quit(); // closes the app
}
