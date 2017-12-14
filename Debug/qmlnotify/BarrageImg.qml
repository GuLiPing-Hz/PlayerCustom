import QtQuick 2.0

//对于主动创建的QQmlComponent不能引入自定义模块 such as : import "xxx.js" as xxx
//Qt.include()

AnimatedImage {
    id: animateImage

    width: 50//可配置
    height: 50//可配置



    source: ""

    function onFinish(){
        if(width == 0)
            width = animateImage.sourceSize.width;
        if(height == 0)
            height = animateImage.sourceSize.height;
    }

    Component.onCompleted: onFinish();
}

