import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Window {
    visible: true
    width: 320
    height: 480
    color: Material.background
    title: qsTr("PixylUpdater")
    opacity: 1

    function pixel(p) {
        return p*2;
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
//            anchors {
//                top: parent.top
//                left: parent.left
//                right: parent.right
//    //            horizontalCenter: parent.horizontalCenter
//            }
            background: Rectangle {
                color: Material.background
                radius: pixel(3)
            }

            TabButton {
                text: qsTr("Apps")
            }

            TabButton {
                text: qsTr("Settings")
            }
        }

        SwipeView {
            id: swipeView
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

//            anchors {
//                top: tabBar.bottom
//                left: parent.left
//                right: parent.right
//                bottom: parent.bottom
//            }

            Item {
                id: appView
                ListView {
                    id: appList
                    keyNavigationWraps: false
                    boundsBehavior: Flickable.StopAtBounds
                    model: appModel
                    delegate: appDelegate
                    anchors {
                        fill: parent
                        margins: pixel(10)
                    }

                    ScrollBar.vertical: ScrollBar {}
                }

                Component {
                    id: appDelegate

                    RowLayout {
                        height: pixel(30)
                        width: parent.width

                        ColumnLayout {
                            Layout.fillWidth: true
                            Text {
                                text: name + " " + version
                                Layout.fillWidth: true
                                font.pixelSize: pixel(10)
                                color: Material.foreground
                            }
                            Text {
                                text: path
                                Layout.fillWidth: true
                                font.pixelSize: pixel(6)
                                color: Material.accent
                            }
                        }
                        Text {
                            id: latestText
                            text: latestVersion
                            font.pixelSize: pixel(10)
                            color: Material.accent

                            NumberAnimation {
                                running: true
                                loops: Animation.Infinite
                                properties: "opacity"
                                target: latestText
                                from: 0.0
                                to: 1.0
                                duration: 1000
                            }
                        }



                        RoundButton {
                            text: "Update"
                            radius: pixel(2)
                        }

                    }


                }

                ListModel {
                    id: appModel
                    ListElement {
                        name: "PixylBooth"
                        version: "0.9"
                        path: "C:/PixylBooth"
                        latestVersion: "1.0"
                    }

                    ListElement {
                        name: "PixylPush"
                        version: "0.5"
                        path: "C:/PixylBooth"
                        latestVersion: "1.0"
                    }

                }
            }

            Item {
                id: settingsView

                ColumnLayout {
                    anchors {
                        fill: parent
                    }
                    Switch {
                        text: "Check Automatically"
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

        Rectangle {
            Layout.fillWidth: true
            height: pixel(50)
            color: "black"
            clip: true

            ListView {
                id: msgList
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar {}

                anchors {
                    fill: parent
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
                        margins: pixel(5)
                    }
                    height: pixel(40)
                    text: time + " " + msg
                    color: Material.foreground
                }
            }

            ListModel {
                id: msgModel

                ListElement {
                    time: "[2019/9/19 9:47 PM]"
                    msg: "Started"
                }
            }
        }


        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            value: 0.5

            NumberAnimation {
                running: true
                loops: Animation.Infinite
                properties: "value"
                target: progressBar
                from: 0.0
                to: 1.0
                duration: 1000
            }
        }
    }






}
