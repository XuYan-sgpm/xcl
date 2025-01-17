# 通用组件库
## v2.0.0
- 并发类接口 (thread, thread_local)
- io类接口 (路径相关接口)
- 日志接口
- 流式接口 (stream相关)

## v2.1.0 (2024/3/6)
- 句柄相关接口 (handle_table 和 name_table接口)，模拟Windows句柄表接口设计
可用于skf和saf句柄相关的实现

## v2.2.0 (2024/11/26)
- 重构atomic接口
- 修改lazy接口
- 新增模块接口
- 重构string接口
- 重构日志模块，新增日志管理
- 新增base64和json接口 
- 新增vector和slice_list接口
- 新增线程池接口
- 修改句柄表，哈希表相关模块