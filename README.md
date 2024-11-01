# PluginCore

## 功能简介
    * 提供插件生命周期管理 - 基于配置 plugin.jsn, 支持动态创建、销毁、默认创建
    * 提供插件数据交互 - 消息、数据发布订阅（进程间）
    * 提供插件调度 - 同步任务、异步任务、定时任务

## 使用说明
    * 登记插件实例至配置 plugin.jsn （默认加载模式）
    * 创建插件 - 接口调用
    * 插件使用

## 配置说明

```json
{
    "defaultLoad" : [
        "PluginID"
    ],
    "plugins" : {
        "PluginID" : {
            "resource" : "",
            "type" : "TypeID",
            "param" : {
            },
            "depends" : ["PluginID"],
            "loads" : ["PluginID"]
        }
    }
}

```

## ID映射表说明

 * 插件ID为 Led 时 索引为 0
 * 类型ID为 Led 时 索引为 0

```json
{
    "PluginID" : {
        "Led" : 0
    },
    "TypeID" : {
        "Led" : 0
    }
}
```
