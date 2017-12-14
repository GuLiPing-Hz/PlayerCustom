.pragma library

//弹幕信息配置

//屏幕宽度
var BarrageWindowWidth = 1280;

//屏幕高度
var BarrageWindowHeight = 720;

//弹幕速度
var speed = 0.05;

//内容高度，这里要求字体高度跟图片的高度相一致
var BarrageContentHeight = 50;

//图片的宽度，单独指定
var BarrageImageWidth = 50;

//弹幕总体高度 - 待定
var BarrageTotalHeight = 60;
//上下两个弹幕的间隙
var BarrageSpacing = 10;
//门店榜
var BubbleShopTop = 42;
//特殊提醒的Y位置
var SpecialY = 100;
//特殊提醒字体高度
var SpecialFontHeight = 30;

//礼物赠送，天天门店弹幕数量限制
var LimitGiftSend = 3;

//弹幕屏幕起始高度
var BarrageInitY = 100;
//送礼弹幕需要增加的高度
var HeightGiftSend = 100;

function createSayObject(parent,name,idx){
    var component = Qt.createComponent("BarrageSay.qml");
    var sayObj = component.createObject(parent, {"name":name,"idx":idx,"height":BarrageTotalHeight});

    if (sayObj == null) {
        // Error Handling
        console.log("Error say creating object");
    }
    return sayObj;
}

function createTextObject(parent,height,isItalic,text,color,size,hAlign,vAlign){
    var component = Qt.createComponent("BarrageText.qml");
    var textObj = component.createObject(parent, {"text":text,"color":color,"font.pixelSize":size
                                             ,"font.italic":isItalic,"hAlign":hAlign
                                             ,"height":height,"vAlign":vAlign});

    if (textObj == null) {
        // Error Handling
        console.log("Error text creating object");
    }
    return textObj;
}

function createImgObject(parent,src,y,width,height) {
    var component = Qt.createComponent("BarrageImg.qml");

    var imgObj = component.createObject(parent, {"y":y,"source":src,"width":width
                                            ,"height":height});

    if (imgObj == null) {
        // Error Handling
        console.log("Error image creating object");
    }
    return imgObj;
}

function createMsgObjectRow(parent,x,y,height,bkg,borderleft,run){
    var component = Qt.createComponent("BarrageMsgRow.qml");
    var msgObj = component.createObject(parent, {"x":x,"y":y,"height":height,"source":bkg
                                            ,"speed":speed,"border.left":borderleft
                                            ,"containerWidth":BarrageWindowWidth,"run":run});

    if (msgObj == null) {
        // Error Handling
        console.log("Error msg row creating object");
    }
    return msgObj;
}

function createMsgObjectColumn(parent,x,y,bkg,run){
    var component = Qt.createComponent("BarrageMsgColumn.qml");
    var msgObj = component.createObject(parent,{"x":x,"y":y,"source":bkg,"speed":speed
                                            ,"run":run});

    if(msgObj == null)
        console.log("Error msg column creating object");
    return msgObj
}

function createSpecialObject(parent){
    var component = Qt.createComponent("SpecialNotify1.qml");
    var specialObj = component.createObject(parent,{"y":SpecialY});

    if(specialObj == null)
        console.log("Error special creating object");
    return specialObj
}

function createDSTObj(parent,giftTimeText,giftNameText,roomRankList){
    var component = Qt.createComponent("DayShopTop.qml");
    var dstObj = component.createObject(parent,{"giftTimeText":giftTimeText,"giftWhatText":giftNameText
                                        ,"roomRankList":roomRankList});

    if(dstObj == null)
        console.log("Error dayshoptop creating object");
    return dstObj
}

function createBarrageBubble(contentObj,height,list,bubbleImg,borderleft){
    if(contentObj != null)
    {
        var str;
        var strs;
        //气泡内容
        var msgObj = createMsgObjectRow(contentObj,0,0,height,bubbleImg,borderleft,false);
        if(msgObj != null)
        {
            for(var i=0;i<list.length;i++)
            {
                var cell = list[i];
                var ts = cell.split("=");
                if(ts.length != 2)
                    continue;

                var type = ts[0];
                if(type == "txt")
                {
                    str = ts[1];//这里包含一个带";"分隔的字符串 例如: "18;#ffffff;文字内容",分别为字体大小;颜色;内容
                    strs = str.split(";");

                    if(strs.length != 3)
                    {
                        console.log("text format length != 3");
                        continue;
                    }

                    createTextObject(msgObj.row,BarrageContentHeight,false,strs[2],strs[1],parseInt(strs[0]),1,2);
                }
                else if(type == "img")
                {
                    str = ts[1];//这里包含一个带";"分隔的字符串 例如: "0;xxx.png"——相对高度;图片
                    strs = str.split(";");
                    if(strs.length != 2)
                        continue;
                    createImgObject(msgObj.row,strs[1],parseInt(strs[0]),BarrageImageWidth,BarrageContentHeight);
                }
            }
            //调整大小
            msgObj.onResize();
        }
    }
}

function createBarrageCommon(parent,x,y,run,funcOnFullShow,funcOnEnd,name,idx,list){
    var totalObj = createMsgObjectRow(parent,x,y,BarrageTotalHeight,"",0,run);
    if(totalObj != null)
    {
        var sayObj = createSayObject(totalObj.row,name,idx);

        //气泡
        var bubbleObj = createBarrageBubble(totalObj.row,BarrageTotalHeight,list,"images/bubbleyellow.png",22);

        if(funcOnFullShow != null)
            totalObj.barrageFullShow.connect(funcOnFullShow);
        if(funcOnEnd != null)
            totalObj.barrageEnd.connect(funcOnEnd);
        totalObj.onResize();
    }
    return totalObj;
}

function createBarragePM(parent,y,funcOnFullShow,name,idx,list){
    return createBarrageCommon(parent,BarrageWindowWidth,y,true,funcOnFullShow,null,name,idx,list);
}

function createBarrageGS(parent,y,funcOnFullShow,funcOnEnd,name,idx,list,location){
    var totalObj = createMsgObjectColumn(parent,BarrageWindowWidth,y,"",true);
    if(totalObj != null)
    {
        var top = createBarrageCommon(totalObj.column,0,0,false,null,null,name,idx,list);
        var strs = location.split(";");
        if(strs.length == 3)//必须要有3个属性
            var bottom = createTextObject(totalObj.column,BarrageContentHeight,false,strs[2],strs[1],strs[0],3,2);

        totalObj.barrageFullShow.connect(funcOnFullShow);
        totalObj.barrageEnd.connect(funcOnEnd);
        totalObj.onResize();
    }

    return totalObj;
}

function createBarrageSTUpdate(parent,y,funcOnFullShow,funcOnEnd,list){
    var offset = 16;
    var totalObj = createMsgObjectRow(parent,BarrageWindowWidth,y+offset,BubbleShopTop,"",true);
    if(totalObj != null)
    {
        var img = createImgObject(totalObj.row,"images/dayshoptop.png",-offset,0,0);
        var bubble = createBarrageBubble(totalObj.row,BubbleShopTop,list,"images/bubbleblack.png",11);

        if(funcOnFullShow != null)
            totalObj.barrageFullShow.connect(funcOnFullShow);
        if(funcOnEnd != null)
            totalObj.barrageEnd.connect(funcOnEnd);
        totalObj.onResize();
    }
    return totalObj;
}

function createSpecialNotify(parent,funcOnEnd,list1,list2){
    var specialObj = createSpecialObject(parent);

    if(specialObj != null)
    {
        var cell;
        var strs;
        for(var i=0;i<list1.length;i++)
        {
            cell = list1[i];
            strs = cell.split(";");//3个
            if(strs.length != 3)
                continue;

            var textObj = createTextObject(specialObj.row1,SpecialFontHeight,true,strs[2],strs[1],parseInt(strs[0]),1,2);
            if(textObj == null)
                console.log("first line create failed");
        }

        for(i=0;i<list2.length;i++)
        {
            cell = list2[i];
            strs = cell.split(";");//3个
            if(strs.length != 3)
                continue;

            textObj = createTextObject(specialObj.row2,SpecialFontHeight,true,strs[2],strs[1],parseInt(strs[0]),1,3);
            if(textObj == null)
                console.log("second line create failed");
        }

        if(funcOnEnd != null)
            specialObj.specialEnd.connect(funcOnEnd);
        specialObj.onResize();
    }

    return specialObj
}

function createDayShopTop(parent,funcOnEnd,giftTimeText,giftNameText,roomRankList){
    var dstObj = createDSTObj(parent,giftTimeText,giftNameText,roomRankList);
    if(dstObj != null)
    {
        if(funcOnEnd != null)
            dstObj.dayShopTopEnd.connect(funcOnEnd);
    }
    return dstObj;
}
