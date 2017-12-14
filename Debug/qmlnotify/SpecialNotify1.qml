import QtQuick 2.0

Item {
    id: container
    x: 1280//配置
    width: 700

    property alias row1: row1
    property alias row2: row2

    property int rightX: 1280
    property int centerX: 350
    property int leftX: -700

    signal specialEnd();

    states: State {
        name: "dead"
    }

    BorderImage {
        id: bgImg
        x: 59
        y: 30
        width: 600
        source: "images/specialbg.png"
        border.top: 21
        border.left: 30
        border.bottom: 15
        border.right: 0
    }

    Image {
        source: "images/star.png"
    }

    Column {
        id: contentTxt
        y: 10
        width: 600
        anchors.left: parent.left
        anchors.leftMargin: 101
        spacing: 8
        opacity: 0

        Row {
            id: row1
        }

        Row {
            id: row2
            anchors.right: parent.right
        }
    }

    SequentialAnimation {
        running: true

        NumberAnimation {
            target: container
            property: "x"
            from: rightX
            to: centerX
            duration: 500
        }
        NumberAnimation {
            target: contentTxt
            property: "opacity"
            from: 0
            to: 1
            duration: 500
        }
        PauseAnimation {
            duration: 4000
        }
        NumberAnimation {
            target: contentTxt
            property: "opacity"
            from: 1
            to: 0
            duration: 1000
        }
        NumberAnimation {
            target: container
            property: "x"
            from: centerX
            to: leftX
            duration: 300
        }
        PropertyAnimation {
            target: container
            property: "state"
            to: "dead"
        }
    }

    onStateChanged: onSC();

    function onSC(){
        if(state == "dead")
        {
            container.specialEnd();
            container.destroy();//释放资源
        }
    }

    function onResize(){
    }
}

