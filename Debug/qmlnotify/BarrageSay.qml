import QtQuick 2.0

Item {
    id: who
    height: 60

    property alias name: textName.text
    property alias idx: textId.text

    Text {
        id: textName
        z: 1
        anchors.top: who.top
        anchors.topMargin: 5
        anchors.right: who.right
        anchors.rightMargin: 5
        height: 30
        text: qsTr("Text")
        font.family: "微软雅黑"
        font.bold: true
        horizontalAlignment: Text.AlignRight
        font.pixelSize: 20
        color: "#ffffff"
    }

    Text {
        anchors.top: who.top
        anchors.topMargin: 5.1
        anchors.right: who.right
        anchors.rightMargin: 4.9
        height: textName.height
        text: textName.text
        font.family: textName.font.family
        font.bold: textName.font.bold
        horizontalAlignment: textName.horizontalAlignment
        font.pixelSize: textName.font.pixelSize
        color: textName.color
    }

    Text {
        id: textId
        z: 1
        anchors.top: textName.bottom
        anchors.right: who.right
        anchors.rightMargin: 5
        height: 20
        text: qsTr("Text2")
        font.family: "微软雅黑"
        font.bold: true
        horizontalAlignment: Text.AlignRight
        font.pixelSize: 12
        color: "#ffffff"
    }

    Text {
        anchors.top: textName.bottom
        anchors.topMargin: 0.1
        anchors.right: who.right
        anchors.rightMargin: 4.9
        height: textId.height
        text: textId.text
        font.family: textId.font.family
        font.bold: textId.font.bold
        horizontalAlignment: textId.horizontalAlignment
        font.pixelSize: textId.font.pixelSize
        color: textId.color
    }

    function onFinished(){
        var maxWidth = textName.width > textId.width ? textName.width : textId.width;
        who.width = maxWidth;
    }

    Component.onCompleted: onFinished();
}

