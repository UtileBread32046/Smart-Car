### 调用小车MCP json指令集

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