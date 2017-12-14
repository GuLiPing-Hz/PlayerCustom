import QtQuick 2.0
import QtQuick.Controls 1.3
import "barrageutil.js" as BarrageUtil

Rectangle {
    id: container
    objectName: "barrageContainer"
    color: "#00000000"

    width: BarrageUtil.BarrageWindowWidth
    height: BarrageUtil.BarrageWindowHeight


    /*
      信息展示的数量限制说明：
        普通弹幕信息无数量限制
        礼物赠送,和门店排名更新通知数量 <= BarrageUtil.ListGiftSend
        天天门店排名展示，得分大于90，礼物兑换当前屏幕同一时刻仅展示一条
    */

    property int nbBarrageNotFullShow: 0
    //统计礼物赠送,和门店排名更新通知
    property int nbActiveBarrage: 0
    property bool isSingleActive: false

    property int nCurY: BarrageUtil.BarrageInitY

    function onAdd(height){
        if(height == 0)
            height = BarrageUtil.BarrageTotalHeight+BarrageUtil.BarrageSpacing
        container.nCurY += height;
        if(container.nCurY+BarrageUtil.BarrageTotalHeight > container.height)
            container.nCurY = BarrageUtil.BarrageInitY;

        nbBarrageNotFullShow++;
    }

    function onBarrageFullShow(barrageY){
        nbBarrageNotFullShow--;
        if(barrageY == BarrageUtil.BarrageInitY)//规则放以后再说。。。
            nCurY = BarrageUtil.BarrageInitY;
    }

    function onGiftSendAdd(height){
        onAdd(height);
        nbActiveBarrage++;
    }

    function onGiftSendEnd(){
        nbActiveBarrage--;//修改状态
        barrage.onBarrageCanAdd();//再调用槽函数
    }

    function onSingleEnd(){
        isSingleActive = false;
        barrage.onSingleEnd();
    }

    function isSActive(){
        return isSingleActive;
    }

    function isBarrageEnough(){
        return (nbActiveBarrage >= BarrageUtil.LimitGiftSend);
    }

    function onAddPMNotify(name,idx,listContent){
        var obj = BarrageUtil.createBarragePM(container,nCurY,onBarrageFullShow,name,idx,listContent);
        onAdd(0);
    }

    function onAddGSNotify(name,idx,list,location){
        var obj = BarrageUtil.createBarrageGS(container,nCurY,onBarrageFullShow,onGiftSendEnd,name,idx,list,location);
        if(obj != null)
        {
            var height = (location == "") ? 0:BarrageUtil.HeightGiftSend;
            onGiftSendAdd(height);
        }
    }

    function onAddSTNotify(list){
        var obj = BarrageUtil.createBarrageSTUpdate(container,nCurY,onBarrageFullShow,onGiftSendEnd,list);
        if(obj != null)
            onGiftSendAdd(0);
    }

    function onAddSPNotify(list1,list2){
        isSingleActive = true;
        var obj = BarrageUtil.createSpecialNotify(container,onSingleEnd,list1,list2);
        if(obj == null)
            isSingleActive = false;
    }

    function onAddT3Notify(giftTimeText,giftNameText,roomRankList){
        isSingleActive = true;
        var obj = BarrageUtil.createDayShopTop(container,onSingleEnd,giftTimeText,giftNameText,roomRankList);
        if(obj == null)
            isSingleActive = false;
    }
}
