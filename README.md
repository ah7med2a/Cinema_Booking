# 🎬 Cinema Management System

A C++ console-based Cinema Management System designed to simulate the core operations of a cinema. The application provides separate Admin and Customer portals, allowing administrators to manage movies, cinema halls, and showtimes while customers can browse movies, check seat availability, and book tickets.

The project focuses on applying **Object-Oriented Programming (OOP)** concepts, STL containers, exception handling, and modern C++ memory management.

##Team
1-Mahmoud Abd Elmohsen (Leader)
2-Mahmoud Mostafa
3-Ahmed Ayman

## ✨ Features

### 🔐 Admin Portal
- Admin registration and authentication.
- Password hashing for credential storage.
- Add new movies with detailed information.
- Edit existing movie information.
- Create cinema halls with customizable capacities.
- Rename existing cinema halls.
- Schedule movie showtimes.
- Prevent overlapping showtimes in the same hall.
- View all movies, halls, and scheduled showtimes.

### 🎟️ Customer Portal
- Browse all available movies.
- Search movies by title or genre.
- View detailed movie information.
- Browse scheduled showtimes.
- View interactive seat availability maps.
- Book tickets by selecting available seats.
- Age-restriction validation before booking.
- Automatic ticket and booking ID generation.
- Display booking and ticket information.

### 🛡️ Validation & Error Handling
- Custom exception classes for different errors.
- Duplicate ID prevention.
- Input validation for dates, times, prices, and other fields.
- Prevention of double-booking seats.
- Showtime conflict detection.
- Robust input handling.

## 🧰 Technologies & Concepts

- **Language:** C++
- **Standard Library:** STL
- **Containers:** `vector`, `map`
- **Memory Management:** `unique_ptr`, `make_unique`
- **OOP Concepts:**
  - Encapsulation
  - Abstraction
  - Inheritance
  - Friend Classes
  - Constructors
  - Access Specifiers
- **Exception Handling:** Custom exceptions and `try-catch`.
- **Algorithms:** STL algorithms such as `count_if` and `transform`.

## 🏗️ Project Structure

The application is organized around the following classes:

| Class | Responsibility |
|---|---|
| `Movie` | Stores movie information and displays movie details. |
| `MovieHall` | Manages cinema halls, capacity, and seats. |
| `Seat` | Represents an individual cinema seat. |
| `ShowTime` | Connects movies with halls and manages reservations. |
| `Ticket` | Stores ticket information. |
| `BookingDetails` | Manages booking records and owns tickets. |
| `Admin` | Handles administrative operations and authentication. |
| `Cinema` | Stores and manages the application's main data. |

## 🚀 Getting Started

### Prerequisites

- A C++ compiler supporting C++14 or later.
- An IDE or code editor such as Visual Studio, VS Code, or Code::Blocks.

### Compilation

Using GCC:

```bash
g++ -std=c++14 -Wall -Wextra -o CinemaManagement main.cpp
```

### Run

**Windows:**

```bash
CinemaManagement.exe
```

**Linux / macOS:**

```bash
./CinemaManagement
```

## 🔑 Default Admin Credentials

The application initializes with a default administrator account.

| Field | Value |
|---|---|
| Admin ID | `1` |
| Password | `admin123` |

You can also register additional administrator accounts through the Admin Portal.

> **Note:** This is an educational project. Data is currently stored in memory and will be reset when the application exits. The password hashing implementation uses `std::hash`, which is not suitable for production authentication systems.

## 📌 Current Limitations

- No database integration.
- No persistent data storage.
- Console-based user interface.
- No ticket cancellation or refund functionality.
- No customer account system.

## 🎯 Project Goals

This project was developed to practice and demonstrate:

- Building a multi-class C++ application.
- Applying OOP principles to a real-world-inspired system.
- Managing object ownership and relationships.
- Using smart pointers instead of manual memory management.
- Designing custom exception hierarchies.
- Implementing validation and business rules.

## 📄 License

This project is available for educational and learning purposes.
