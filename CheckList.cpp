/**
 * @file CheckList.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Task.h"
#include "CheckList.h"

/**
 * @brief CheckList method of CheckList.cpp.
 * @param checkListValidity(true parameter for CheckList.
 */
CheckList::CheckList() : checkListValidity(true) {}

// Add a new task to the list by task description
/**
 * @brief addTask method of CheckList.cpp.
 * @param taskDesc parameter for addTask.
 */
void CheckList::addTask(std::string taskDesc) {
    Task newTask = Task(taskDesc);  // Create a new task
    taskList.push_back(newTask);   // Add task to the vector
}

// Remove a task from the list by index
/**
 * @brief removeTask method of CheckList.cpp.
 * @param index parameter for removeTask.
 */
void CheckList::removeTask(int index) {
    if (index >= 0 && index < taskList.size()) {
        taskList.erase(taskList.begin() + index); // Remove task at given index
    } else {
        std::cerr << "Invalid task index: " << index << "\n";
    }
}

// Update a specific task's status (e.g., mark as complete)
/**
 * @brief updateCheck method of CheckList.cpp.
 * @param index parameter for updateCheck.
 * @param status parameter for updateCheck.
 */
void CheckList::updateCheck(int index, bool status) {
    if (index >= 0 && index < taskList.size()) {
        taskList[index].updateTask(status); // Update the task status
    } else {
        std::cerr << "Invalid task index: " << index << "\n";
    }
}

// Save tasks to a file
/**
 * @brief saveTasks method of CheckList.cpp.
 */
void CheckList::saveTasks() {
    std::ofstream file("tasks.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file for saving tasks.\n";
        return;
    }

    for (Task& task : taskList) {
        file << task.getTaskDesc() << "," << task.isTaskComplete() << "\n";
    }

    file.close();
}

// Load tasks from a file
/**
 * @brief loadTasks method of CheckList.cpp.
 */
void CheckList::loadTasks() {
    taskList.clear();
    std::ifstream file("tasks.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file for loading tasks.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            std::string desc = line.substr(0, commaPos);
            bool status = (line.substr(commaPos + 1) == "1");
            addTask(desc);                     // Add the task to the list
            updateCheck(taskList.size() - 1, status);  // Update its status
        }
    }

    file.close();
}

/**
 * @brief getTasks method of CheckList.cpp.
 * @return Result of the operation.
 */
std::vector<Task>& CheckList::getTasks() {
    return taskList;
}