/**
 * @file Task.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "Task.h"

// Constructor with title only
/**
 * @brief Task method of Task.cpp.
 * @param taskDesc(newDesc) parameter for Task.
 * @param taskComplete(false parameter for Task.
 */
Task::Task(const std::string& newDesc) : taskDesc(newDesc), taskComplete(false) { }

/**
 * @brief updateTask method of Task.cpp.
 * @param newStatus parameter for updateTask.
 */
void Task::updateTask(bool newStatus) {
    taskComplete = newStatus;
}

/**
 * @brief getTaskDesc method of Task.cpp.
 * @return Result of the operation.
 */
std::string Task::getTaskDesc() {
    return taskDesc;
}

/**
 * @brief isTaskComplete method of Task.cpp.
 * @return Result of the operation.
 */
bool Task::isTaskComplete() {
    return taskComplete;
}
