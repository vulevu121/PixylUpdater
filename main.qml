import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0
import GetSettings 1.0
import Firebase 1.0

Window {
    id: mainWindow
    visible: true
    width: 320
    height: 480
    color: Material.background
    title: qsTr("PixylUpdater")
    opacity: 1

    property string idToken
    property string refreshToken

    function pixel(p) {
        return p*2;
    }

    Firebase {
        id: firebase

        idToken: mainWindow.idToken
        refreshToken: mainWindow.refreshToken

        onAuthenticationSuccess: {
            progressBar.stop()
        }

        onAuthenticationError: {
            progressBar.stop()
        }

        onRequestError: {
//            console.log(msg)
//            msgModel.append({"msg": msg})

            msgModel.appendMsg(msg)
        }

        onUserInfoReceived: {
            console.log("User info received")
        }

        onStatus: {
//            msgModel.append({"msg": msg})
            msgModel.appendMsg(msg)
        }



    }

    GetSettings {
        id: getSettings
    }

    Timer {
        interval: 500
        running: true
        onTriggered: {
            progressBar.run()
            var app = "PixylBooth"
            appModel.append({
                                "name": app,
                                "exePath": getSettings.exePath(app),
                                "version": getSettings.version(app),
                                "latestVersion": ""
                            })
            app = "PixylPush"
            appModel.append({
                                "name": app,
                                "exePath": getSettings.exePath(app),
                                "version": getSettings.version(app),
                                "latestVersion": ""
                            })

            firebase.authenticate("vulevu121@gmail.com", "V73911937l")
        }
    }


    ColumnLayout {
        anchors {
            fill: parent
        }
        spacing: 0

        TabBar {
            id: tabBar
            position: TabBar.Header
            currentIndex: swipeView.currentIndex
            Layout.fillWidth: true

            background: Rectangle {
                color: Material.background
                radius: pixel(3)
            }

            TabButton {
                text: qsTr("Apps")
                icon.source: "qrc:/icons/apps"
            }

            TabButton {
                text: qsTr("Settings")
                icon.source: "qrc:/icons/tune"
            }
        }

        SwipeView {
            id: swipeView
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true


            // ====== App View ======
            Item {
                id: appView

                Rectangle {
                    anchors {
                        fill: parent
//                        margins: pixel(10)
                    }
                    color: "transparent"
                    clip: true
                    ListView {
                        id: appList
                        keyNavigationWraps: false
    //                    boundsBehavior: Flickable.StopAtBounds
                        model: appModel
                        delegate: appDelegate

                        anchors {
                            fill: parent
    //                        margins: pixel(10)
                        }
                        ScrollBar.vertical: ScrollBar {}
                    }
                }


                Component {
                    id: appDelegate

                    ColumnLayout {
                        height: pixel(30)
                        width: parent.width

                        Connections {
                            target: firebase
                            onDocumentReceived: {
                                var appName = String(name)
                                var path = String(json["name"])
//                                console.log(json["fields"])
                                var latestVersion = String(json["fields"]["latestVersion"]["stringValue"])
                                var latestDownload = String(json["fields"]["latestDownload"]["stringValue"])
                                if (path.indexOf("/apps/" + appName.toLowerCase()) >= 0) {
                                    appModel.set(index, {"latestVersion": latestVersion})
                                    appModel.set(index, {"latestDownload": latestDownload})
                                }
                            }

                            onDownloadCompleted: {
                                var appName = String(name)
                                var path = String(savePath)

                                if (path.indexOf(appName) >= 0) {
                                    appProgressBar.stop()
//                                    getSettings.setVersion(appName, String(latestVersion))
//                                    appModel.set(index, {"version": getSettings.version(appName)})
                                }

                            }

                            onRequestError: {
                                appProgressBar.stop()
                            }

                        }

                        Timer {
                            id: checkTimer
                            interval: (index + 1) * 1000
                            repeat: true
                            running: true

                            onTriggered: {
                                var appName = String(appModel.get(index).name)
                                var latestVersion = String(appModel.get(index).latestVersion)

                                if (latestVersion.length === 0) {
                                    firebase.getDocument("/apps/" + appName.toLowerCase())
                                }
                                else {
                                    checkTimer.stop()
                                }

                            }

                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.leftMargin: pixel(4)

                                Text {
                                    text: name + " " + version
                                    Layout.fillWidth: true

                                    font.pixelSize: pixel(10)
                                    color: Material.foreground
                                }
                                Text {
                                    text: exePath
                                    Layout.fillWidth: true
                                    font.pixelSize: pixel(6)
                                    color: Material.accent
                                    elide: Text.ElideMiddle

                                }
                            }

                            Text {
                                id: latestText
                                text: latestVersion
                                font.pixelSize: pixel(10)
                                color: Material.accent



    //                            NumberAnimation {
    //                                running: true
    //                                loops: Animation.Infinite
    //                                properties: "opacity"
    //                                target: latestText
    //                                from: 0.0
    //                                to: 1.0
    //                                duration: 1000
    //                            }
                            }

                            RoundButton {
                                text: "Update"
                                radius: pixel(2)
                                onClicked: {
                                    appProgressBar.run()
                                    var latestDownload = appModel.get(index).latestDownload
                                    var exePath = appModel.get(index).exePath
                                    firebase.download(latestDownload, exePath)
                                }


                            }

                        }
                        ProgressBar {
                            id: appProgressBar
                            height: pixel(4)
                            Layout.fillWidth: true
                            value: 0

                            property bool running: false

                            function run() {
                                running = true
                            }

                            function stop() {
                                running = false
                                value = 0
                            }

                            NumberAnimation {
                                running: appProgressBar.running
                                loops: Animation.Infinite
                                properties: "value"
                                target: appProgressBar
                                from: 0.0
                                to: 1.0
                                duration: 1000
                            }


                        }
                    }


                }

                ListModel {
                    id: appModel

                }
            }

            // ======= Settings View =======
            Item {
                id: settingsView

                ColumnLayout {
                    anchors {
                        fill: parent
                    }
                    Switch {
                        text: "Check Periodically"
                        Layout.fillWidth: true
                    }

                    Switch {
                        text: "Auto Update"
                        Layout.fillWidth: true
                    }


                    RowLayout {}
                }
            }

        }

        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            value: 0

            property bool running: false

            function run() {
                running = true
            }

            function stop() {
                running = false
                value = 0
            }

            NumberAnimation {
                running: progressBar.running
                loops: Animation.Infinite
                properties: "value"
                target: progressBar
                from: 0.0
                to: 1.0
                duration: 1000
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: pixel(50)
            color: "black"
            opacity: 0.5
            clip: true

            ListView {
                id: msgList
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar {}

                anchors {
                    fill: parent
//                    margins: pixel(1)
                }

                model: msgModel
                delegate: msgDelegate
            }

            Component {
                id: msgDelegate

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: pixel(1)
                    }
//                    height: pixel(40)
                    text: msg
                    color: Material.foreground
                }
            }

            ListModel {
                id: msgModel

                function appendMsg(msg) {
                    msgModel.append({"msg": msg})
                }

//                ListElement {
//                    time: "[2019/9/19 9:47 PM]"
//                    msg: "Started"
//                }
            }
        }



    }






}
