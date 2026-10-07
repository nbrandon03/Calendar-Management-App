/**
 * @file Task.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef TASK_H
#define TASK_H

#include <string>

/**
 * @brief Class Task.
 */
class Task {
    private:
        std::string taskDesc{};
        bool taskComplete{};

    public:
        // Constructor with title only
/**
 * @brief Task method of Task.h.
 * @param newDesc parameter for Task.
 */
        Task(const std::string& newDesc);

/**
 * @brief updateTask method of Task.h.
 * @param newStatus parameter for updateTask.
 */
        void updateTask(bool newStatus);

/**
 * @brief getTaskDesc method of Task.h.
 * @return Result of the operation.
 */
        std::string getTaskDesc();

/**
 * @brief isTaskComplete method of Task.h.
 * @return Result of the operation.
 */
        bool isTaskComplete();

    };

#endif