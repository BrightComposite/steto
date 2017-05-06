# Полезные модули для Qt
 
#### Чтобы подключить конкретный модуль:
Добавить `include(path-to-module/module-name.pri)` в pro-file  

#### Чтобы модули подхватывались в Qt:
Добавить `engine.addImportPath(QStringLiteral("qrc:/ru/applabs/"));` в main.cpp  

## Tasks

### Task
#### enum Status 
* NOT_FINISHED
* SUCCEEDED
* FAILED
* CANCELED  

**Свойства:**
* ***data*** : any - данные, привязанные к задаче
* ***status*** : Task.Status  - текущий статус задачи  

**Методы:**  
* ***succeed()*** : void - завершить задачу (успешно) завершена  
* ***fail()*** : void - завершить задачу (неудачно)  
* ***cancel()*** : void - отменить задачу  
---
### Scheduler 
**Методы:**
* ***task([worker, data])*** : ScheduleBuilder - создает объект **ScheduleBuilder**.
    * *worker*: function (**Task** t)
    * *data*: any
---
### ScheduleBuilder  
**Методы:**
* ***then(worker[, data])*** : ScheduleBuilder - добавляет задачу в расписание.
    * *worker*: function (**Task** t)
    * *data*: any

* ***and(worker[, data])*** : ScheduleBuilder - добавляет задачу в для параллельного исполнения вместе с предыдущей.
    * *worker*: function (**Task** t)
    * *data*: any

* ***timeout(t)*** : ScheduleBuilder - применяет таймаут к последней добавленной задаче, по истечении этого времени задача фейлится
    * *t*: int - время в миллисекундах

* ***success(callback)*** : ScheduleBuilder - добавляет коллбэк для успешного выполнения. Коллбэк выполняется после каждой задачи.
    * *callback*: function (**Task** t, **ScheduleBuilder** s)

* ***fail(callback)*** : ScheduleBuilder - добавляет коллбэк при неудачного выполнения. Коллбэк выполняется после первой зафейлившейся задачи.
    * *callback*: function ( **Task** t, **ScheduleBuilder** s)

* ***run()*** : void - запускает расписание

* ***finished()*** : bool - проверяет, завершилось ли расписание полностью (является ли текущая задача последней?)  

* ***stop()*** : void - останавливает выполнение расписания и очищает список задач

* ***repeat([timeout])*** : void - повторяет последнюю задачу (иногда полезно в случае неудачного выполнения)
    * *timeout*: int - время, через которое задача должна быть выполнена повторно (если !timeout, то задача будет выполнена немедленно) 

* ***count()*** : int - возвращает количество задач, оставшихся в очереди