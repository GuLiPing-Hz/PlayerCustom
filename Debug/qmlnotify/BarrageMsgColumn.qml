import QtQuick 2.0

Item {
    id: singleContainer
    property alias column: column
    property alias run : timer.running

    property real lastMs: 0
    property real speed: 0.05//可配置
    property int containerWidth: 1280//可配置
    //x: 1280//可配置
    y: 0
    //height: 60

    property bool fullAdvise: false
    //弹幕全部展示时的信号,只提示一次
    signal barrageFullShow(real barrageY)
    //弹幕消失之后的信号
    signal barrageEnd()

    Column {
        id: column
        anchors.right: parent.right//靠右对齐
    }

    Timer {
        id: timer
        interval: 30
        repeat: true
        triggeredOnStart: false
        running: true

        onTriggered: singleContainer.moveToLeft()
    }

    //平滑向左移动
    function moveToLeft(){

        var d = new Date();
        var curMs = d.getTime();
        if(singleContainer.lastMs == 0)
            singleContainer.lastMs = curMs;

        singleContainer.x -= singleContainer.speed*(curMs-singleContainer.lastMs);
        singleContainer.lastMs = curMs;

        if(!singleContainer.fullAdvise && singleContainer.x+singleContainer.width <= singleContainer.containerWidth)
        {
            singleContainer.barrageFullShow(singleContainer.y);
            singleContainer.fullAdvise = true;
        }

        if(singleContainer.x+singleContainer.width < 0)
        {
            singleContainer.x = singleContainer.containerWidth;
            singleContainer.lastMs = 0;
            //停止定时器
            timer.stop();
            //发送结束信号
            singleContainer.barrageEnd();
            //销毁自己
            singleContainer.destroy();
        }
    }

    function onResize(){
        if(column.children.length != 0)
        {
            for(var i=0;i<column.children.length;i++)
            {
                if(i == 0)
                    singleContainer.width = column.children[i].width;
                else if(i == 1)
                    column.children[i].width = singleContainer.width;
                singleContainer.height += column.children[i].height;
            }
        }
    }
}



