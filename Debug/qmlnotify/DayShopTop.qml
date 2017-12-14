import QtQuick 2.0

BorderImage {
    id: container

    property alias giftTimeText: rowGiftTime.text
    property alias giftWhatText: rowGiftText.text
    property string roomRankList: ""

    x: 900
    width: 340
    height: 210
    opacity: 0

    source: "images/dayshoptopbg.png"
    border.top: 8
    border.left: 8
    border.right: 8
    border.bottom: 8

    signal dayShopTopEnd()

    states: State {
        name: "dead"
    }

    Image {
        x: 5
        y: -30
        source: "images/dayshoptop.png"
    }

    BarrageText {
        y: -5
        anchors.right: parent.right
        anchors.rightMargin: 10
        text: "当前前三甲"
        font.pixelSize: 35
        color: "#ffffff"
    }

    Row {
        id: rowGift
        x: 30
        anchors.top: parent.top
        anchors.topMargin: 60

        BarrageText {
            height: 20
            text: "坚持到"
            color: "#ffdd10"
            font.pixelSize: 15
        }
        BarrageText {
            id: rowGiftTime
            height: 20
            //text: "22点33分"
            color: "#ffdd10"
            font.pixelSize: 15
        }
        BarrageText {
            height: 20
            text: ",前三甲有  "
            color: "#ffdd10"
            font.pixelSize: 15
        }
        BarrageText {
            id: rowGiftText
            height: 20
            //text: "北京烤鸭"
            color: "#ffffff"
            font.pixelSize: 15
        }
        BarrageText {
            height: 20
            text: "  奖励"
            color: "#ffdd10"
            font.pixelSize: 15
        }

    }

    Column {
        x: 30
        anchors.top: rowGift.bottom
        anchors.topMargin: 20
        spacing: 5
        Row {
            id: rowFirst
            BarrageText {
                height: 30
                text: "第一名  包厢"
                color: "#ffffff"
                font.pixelSize: 25
            }
            BarrageText {
                id: roomNameFirst
                height: 30
                //text: "A11"
                color: "#ffffff"
                font.pixelSize: 25
            }
            BarrageText {
                id: roomScoreFirst
                height: 30
                //text: "  90.0"
                color: "#ffdd10"
                font.pixelSize: 25
            }
            BarrageText {
                height: 30
                text: "分"
                color: "#ffffff"
                font.pixelSize: 25
            }
        }
        Row {
            id: rowSecond
            BarrageText {
                height: 30
                text: "第二名  包厢"
                color: "#ffffff"
                font.pixelSize: 25
            }
            BarrageText {
                id: roomNameSecond
                height: 30
                //text: "A11"
                color: "#ffffff"
                font.pixelSize: 25
            }
            BarrageText {
                id: roomScoreSecond
                height: 30
                //text: "  90.0"
                color: "#ffdd10"
                font.pixelSize: 25
            }
            BarrageText {
                height: 30
                text: "分"
                color: "#ffffff"
                font.pixelSize: 25
            }
        }
        Row {
            id: rowThird
            BarrageText {
                height: 30
                text: "第三名  包厢"
                color: "#ffffff"
                font.pixelSize: 25
            }
            BarrageText {
                id: roomNameThird
                height: 30
                //text: "A11"
                color: "#ffffff"
                font.pixelSize: 25
            }
            BarrageText {
                id: roomScoreThird
                height: 30
                //text: "  90.0"
                color: "#ffdd10"
                font.pixelSize: 25
            }
            BarrageText {
                height: 30
                text: "分"
                color: "#ffffff"
                font.pixelSize: 25
            }
        }
    }

    SequentialAnimation {
        running: true
        ParallelAnimation {
            NumberAnimation {
                target: container
                property: "opacity"
                from: 0
                to: 1
                duration: 500
            }
            NumberAnimation {
                target: container
                property: "y"
                from: -210
                to: 50
                duration: 500
            }
        }

        PauseAnimation {
            duration: 4000
        }
        NumberAnimation {
            target: container
            property: "opacity"
            from: 1
            to: 0
            duration: 1000
        }
        PropertyAnimation {
            target: container
            property: "state"
            to: "dead"
            duration: 0
        }
    }

    function onSC(){
        if(state == "dead")
        {
            container.dayShopTopEnd();
            container.destroy();
        }
    }

    function onFinish(){
        var rows = new Array();
        rows[0] = rowFirst;
        rows[1] = rowSecond;
        rows[2] = rowThird;
        var roomName = new Array();
        roomName[0] = roomNameFirst;
        roomName[1] = roomNameSecond;
        roomName[2] = roomNameThird;
        var roomScore = new Array();
        roomScore[0] = roomScoreFirst;
        roomScore[1] = roomScoreSecond;
        roomScore[2] = roomScoreThird;

        var i;
        if(roomRankList == "")//这里过滤掉没用任何名次的时候
        {
            for(i=0;i<3;i++)
                rows[i].opacity = 0;
        }
        else
        {
            var strs = roomRankList.split(";");// "A11=90.0;B11=80.5;C11=70.2";
            var j = strs.length;
            for(i=0;i<3;i++)
            {
                if(i<j)
                {
                    var cell = strs[i];
                    var room_score = cell.split("=");
                    roomName[i].text = room_score[0];
                    roomScore[i].text = "  "+room_score[1];
                }
                else
                    rows[i].opacity = 0;
            }
        }
    }

    onStateChanged: onSC();
    Component.onCompleted: onFinish();
}
