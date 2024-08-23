
# ConcurrentNewsSystem: Concurrent News Broadcasting System

## Project Overview

This project is a part of my coursework in Operating Systems, where I explored the intricacies of thread synchronization in concurrent programming using semaphores. The project simulates a news broadcasting system where multiple threads interact to produce, sort, edit, and display news stories in real-time.

## Key Features

- **Multi-threading**: The system utilizes multiple threads, each representing different components in a news broadcasting system.
- **Thread Synchronization**: The threads communicate through bounded and unbounded queues, protected by semaphores to ensure safe access to shared resources.
- **Simulation of Real-Time Processes**: Producers generate news stories of various types, which are then sorted, edited, and displayed on the screen in a synchronized manner.

## System Components

1. **Producers**: 
   - Each producer generates a specific number of news stories of different types (e.g., SPORTS, NEWS, WEATHER).
   - The stories are then sent to a Dispatcher via private queues.

2. **Dispatcher**: 
   - Monitors the producers' queues and sorts the stories into category-specific queues.
   - Uses a Round Robin algorithm to ensure fair processing.

3. **Co-Editors**: 
   - Each type of story has a dedicated Co-Editor.
   - Co-Editors receive stories, "edit" them (simulated by a brief delay), and pass them to the Screen Manager.

4. **Screen Manager**: 
   - Receives edited stories and displays them on the screen.
   - After processing all stories, it displays a ‘DONE’ message.

## Configuration File

The system's behavior is driven by a configuration file that specifies:

- The number of stories each producer generates.
- The size of the queues used for communication between the threads.
- The size of the shared queue for Co-Editors.

**Example Configuration File**:

\`\`\`
1
10
5

2
10
4

3
10
8

10
\`\`\`

This configuration specifies:
- Producer 1 generates 10 stories and uses a queue of size 5.
- Producer 2 generates 10 stories and uses a queue of size 4.
- Producer 3 generates 10 stories and uses a queue of size 8.
- The shared queue for Co-Editors is of size 10.

## How to Run

1. **Clone the Repository**:
   \`\`\`bash
   git clone https://github.com/hodamarr/ConcurrentNewsSystem.git
   cd SyncNews
   \`\`\`

2. **Compile the Code**:
   \`\`\`bash
   make
   \`\`\`

3. **Run the Program**:
   \`\`\`bash
   ./ex3.out config.txt
   \`\`\`

4. **Configuration**:
   - Modify `config.txt` to adjust the number of stories and queue sizes based on your simulation needs.

## Technologies Used

- **C++**: The core logic is implemented in C++.
- **POSIX Threads**: The project uses POSIX threads for multi-threading.
- **Semaphores**: Semaphores are used for synchronization to protect shared resources.

## Project Structure

- **ex3.cpp**: Contains the implementation of the Producers, Dispatcher, Co-Editors, and Screen Manager.
- **config.txt**: Example configuration file to control the system's behavior.
- **makefile**: Automates the compilation process.

## Future Enhancements

- **Dynamic Configuration**: Extend the system to adjust queue sizes based on workload dynamically.
- **Logging**: Implement a logging mechanism to track thread activities for debugging purposes.
- **GUI**: Develop a graphical interface to visualize the process flow in real-time.

