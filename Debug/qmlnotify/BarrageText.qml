import QtQuick 2.0

Text {
    id: text

    property int vAlign: 2//默认垂直居中
    property int hAlign: 1//默认水平靠左
    height: 50//可配置
    font.family: "微软雅黑"
    font.bold: true
    font.italic: false
    horizontalAlignment: Text.AlignRight
    verticalAlignment: Text.AlignVCenter

    text: ""
    color: "#000000"
    font.pixelSize: 18

    function onFinish(){
        text.horizontalAlignment = (hAlign==2?Text.AlignHCenter:(hAlign==3?Text.AlignRight:Text.AlignLeft));
        text.verticalAlignment = (vAlign==1?Text.AlignTop:(vAlign==3?Text.AlignBottom:Text.AlignVCenter));
    }

    Component.onCompleted: onFinish();
}

