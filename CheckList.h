/**
 * @file CheckList.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef CHECKLIST_H
#define CHECKLIST_H

#include <string>
#include <vector>
#include "Task.h"

/**
 * @brief Class CheckList.
 */
class CheckList {
    private:
        std::vector<Task> taskList;
        bool checkListValidity;

    public:
/**
 * @brief CheckList method of CheckList.h.
 */
        CheckList();

/**
 * @brief addTask method of CheckList.h.
 * @param newTask parameter for addTask.
 */
        void addTask(std::string newTask);

/**
 * @brief removeTask method of CheckList.h.
 * @param index parameter for removeTask.
 */
        void removeTask(int index);

/**
 * @brief updateCheck method of CheckList.h.
 * @param index parameter for updateCheck.
 * @param status parameter for updateCheck.
 */
        void updateCheck(int index, bool status);

/**
 * @brief saveTasks method of CheckList.h.
 */
        void saveTasks();

/**
 * @brief loadTasks method of CheckList.h.
 */
        void loadTasks();

/**
 * @brief getTasks method of CheckList.h.
 * @return Result of the operation.
 */
        std::vector<Task>& getTasks();


};

#endif