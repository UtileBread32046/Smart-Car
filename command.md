# 🕹️ Smart-Car 指令集手册

## 📑 快速导航索引
[📥 终端字符串指令 (Serial/PC)](#终端字符串指令)

---

## 终端字符串指令
可以直接在串口监视器（Serial Monitor）中输入以下json指令

1. 全局指令集
> 用于调节小车的全局状态

| 指令类别 | 指令字符 | 可选参数 | 功能描述 | Json 指令 |
| :--- | :--- | :--- | :--- | :--- |
| **状态控制** | `setRunning` | `isRunning:true/false` | 控制小车运行状态 | `{"cmd":"setRunning", "params":{"isRunning":true}}` |
| **模式切换** | `setMoveMode` | `mode:DIFF/ANGLE/TRACK` |选择不同的控制模式 | `{"cmd":"setMoveMode","params":{"mode":"DIFF"}}` |


2. 测试指令集
> 用于单独测试小车的某项功能

| 指令类别 | 指令字符 | 可选参数 | 功能描述 | Json 指令 |
| :--- | :--- | :--- | :--- | :--- |
| **定量转体** | `turn` | `turnAngle:90` | 使小车原地旋转指定角度(以顺时针为正) | `{"cmd":"turn","params":{"turnAngle":90}}` |
| **定时移动** | `run_time` | `direction:forward/backward ; time:1000 ; speed:50` | 在指定时间(单位:ms)内以指定速度向指定方向移动 | `{"cmd":"run_time","params":{"direction":"forward","time":500,"speed":80}}` |
| **定长移动** | `run_distance` | `distance:50 ; speed:50/-50` | 以指定速度移动指定距离(单位:mm) | `{"cmd":"run_distance","params":{"distance":-50,"speed":60}}` |
| **闭环控制** | `lock/unlock` | - | 启动/禁用角度锁定模式 | `{"cmd":"lock"}` |
| **最大速度** | `setMaxSpeed` | `speed:50` | 设置小车的最大速度 | `{"cmd":"setMaxSpeed", "params":{"speed":50}}` |
---

### [原版]调用小车MCP json指令集

1. 转向指令
    a. 指令名称: turn
    b. 可调参数: 转向角度
````json
{"cmd":"turn","params":{"turnAngle":90}}
````

2. 定时移动指令
    a. 指令名称: run_time
    b. 可调参数: 方向, 运行时间, 运行速度
````json
{"cmd":"run_time","params":{"direction":"forward","time":500,"speed":80}}
````

3. 定长移动指令
    a. 指令名称: run_distance
    b. 可调参数: 移动距离, 运行速度
````json
{"cmd":"run_distance","params":{"distance":-50,"speed":60}}
````

4. 闭环控制指令
    a. 指令名称: lock / unlock
````json
{"cmd":"lock"}
{"cmd":"unlock"}
````

5. 设置最大速度指令
    a. 指令名称: setMaxSpeed
    b. 可调参数: 最大速度
````json
{"cmd":"setMaxSpeed", "params":{"speed":50}}
````

6. 设置运行状态指令
    a. 指令名称: setRunning
    b. 可调参数: 运行状态
````json
{"cmd":"setRunning", "params":{"isRunning":true}}
{"cmd":"setRunning", "params":{"isRunning":false}}
````

7. 切换模式指令
    a. 指令名称: setMoveMode
    b. 可调参数: 运动模式
````json
{"cmd":"setMoveMode","params":{"mode":"DIFF"}}
{"cmd":"setMoveMode","params":{"mode":"ANGLE"}}
{"cmd":"setMoveMode","params":{"mode":"TRACK"}}
````
