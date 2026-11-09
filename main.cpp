#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <memory>
#include <exception>
#include <cctype>

using namespace std;

// ============================================================
// Custom Exceptions
// ============================================================
class CinemaException : public exception {
protected:
    string message;
public:
    explicit CinemaException(string msg) : message(std::move(msg)) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InvalidCredentialsException : public CinemaException {
public:
    InvalidCredentialsException() : CinemaException("Error: Invalid ID or password!") {}
};

class AdminAlreadyExistsException : public CinemaException {
public:
    AdminAlreadyExistsException() : CinemaException("Error: Admin ID already exists!") {}
};

class NotFoundException : public CinemaException {
public:
    explicit NotFoundException(const string& item) : CinemaException("Error: " + item + " not found!") {}
};

class SeatAlreadyReservedException : public CinemaException {
public:
    SeatAlreadyReservedException() : CinemaException("Error: Selected seat is already reserved!") {}
};

// New exceptions (fixes #2, #3, #6)
class DuplicateIdException : public CinemaException {
public:
    DuplicateIdException(const string& item, int id)
        : CinemaException("Error: " + item + " ID " + to_string(id) + " already exists!") {}
};

class InvalidInputException : public CinemaException {
public:
    explicit InvalidInputException(const string& msg) : CinemaException("Error: " + msg) {}
};

class ShowTimeConflictException : public CinemaException {
public:
    ShowTimeConflictException()
        : CinemaException("Error: This hall already has a showtime overlapping that time slot!") {}
};

class AgeRestrictedException : public CinemaException {
public:
    explicit AgeRestrictedException(int minAge)
        : CinemaException("Error: This movie is restricted to viewers aged " + to_string(minAge) + "+.") {}
};

// Thrown when stdin is closed; not a CinemaException.
struct InputClosed {};

// ============================================================
// Validation helpers
// ============================================================
static int parseTimeToMinutes(const string& t) {          // "HH:MM"
    if (t.size() != 5 || t[2] != ':' ||
        !isdigit((unsigned char)t[0]) || !isdigit((unsigned char)t[1]) ||
        !isdigit((unsigned char)t[3]) || !isdigit((unsigned char)t[4]))
        throw InvalidInputException("Time must be in HH:MM format (e.g. 18:30).");
    int h = stoi(t.substr(0, 2)), m = stoi(t.substr(3, 2));
    if (h > 23 || m > 59) throw InvalidInputException("Time is out of range (00:00 - 23:59).");
    return h * 60 + m;
}

static void validateDate(const string& d) {               // "YYYY-MM-DD"
    bool ok = d.size() == 10 && d[4] == '-' && d[7] == '-';
    for (size_t i = 0; ok && i < d.size(); ++i)
        if (i != 4 && i != 7 && !isdigit((unsigned char)d[i])) ok = false;
    if (ok) {
        int mo = stoi(d.substr(5, 2)), dy = stoi(d.substr(8, 2));
        ok = mo >= 1 && mo <= 12 && dy >= 1 && dy <= 31;
    }
    if (!ok) throw InvalidInputException("Release date must be in YYYY-MM-DD format.");
}

static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)tolower(c); });
    return s;
}
static string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)toupper(c); });
    return s;
}

// ============================================================
// Forward Declarations
// ============================================================
class Admin;
class MovieHall;
class ShowTime;
class Seat;

// ============================================================
// Movie
// ============================================================
class Movie {
    int movieId;
    int duration;
    int ageRating;
    double price;
    string title;
    string releaseDate;
    string genre;
    string language;
    string description;

public:
    Movie(int movieId, int duration, string releaseDate, double price, int ageRating,
          string title, string genre, string language,
          string description = "No description available.")
        : movieId(movieId), duration(duration), ageRating(ageRating), price(price),
          title(std::move(title)), releaseDate(std::move(releaseDate)), genre(std::move(genre)),
          language(std::move(language)), description(std::move(description)) {}

    int getId() const { return movieId; }
    int getDuration() const { return duration; }
    double getPrice() const { return price; }
    int getAgeRating() const { return ageRating; }
    string getTitle() const { return title; }
    string getReleaseDate() const { return releaseDate; }
    string getGenre() const { return genre; }
    string getLanguage() const { return language; }
    string getDescription() const { return description; }

    void display() const {
        cout << "[ID: " << movieId << "] " << title << " (" << genre << ") - "
             << duration << " mins | Age: +" << ageRating << " | Price: $"
             << fixed << setprecision(2) << price << " | Lang: " << language << "\n";
    }

    // Fix #9: description is now visible to customers.
    void displayDetails() const {
        cout << "\n===== " << title << " =====\n"
             << "Movie ID    : " << movieId << "\n"
             << "Genre       : " << genre << "\n"
             << "Language    : " << language << "\n"
             << "Duration    : " << duration << " mins\n"
             << "Age Rating  : +" << ageRating << "\n"
             << "Released    : " << releaseDate << "\n"
             << "Ticket Price: $" << fixed << setprecision(2) << price << "\n"
             << "Description : " << description << "\n";
    }

protected:
    friend class Admin;
    void setId(int v) { movieId = v; }
    void setDuration(int v) { duration = v; }
    void setPrice(double v) { price = v; }
    void setAgeRating(int v) { ageRating = v; }
    void setTitle(string v) { title = std::move(v); }
    void setReleaseDate(string v) { releaseDate = std::move(v); }
    void setGenre(string v) { genre = std::move(v); }
    void setLanguage(string v) { language = std::move(v); }
    void setDescription(string v) { description = std::move(v); }
};

// ============================================================
// Seat
// ============================================================
class Seat {
    int seatId;
    string seatNumber; // e.g. "A1"

public:
    Seat(int seatId, string seatNumber) : seatId(seatId), seatNumber(std::move(seatNumber)) {}

    int getSeatId() const { return seatId; }
    string getSeatNumber() const { return seatNumber; }

protected:
    friend class Admin;
    void setId(int v) { seatId = v; }
    void setSeatNumber(string v) { seatNumber = std::move(v); }
};

// ============================================================
// MovieHall  (Fix #1, #5: the hall now owns its own seats, one per unit of capacity)
// ============================================================
class MovieHall {
public:
    static const int SEATS_PER_ROW = 10;
    static const int MAX_CAPACITY = 260; // 26 rows x 10

private:
    int hallId;
    int capacity;
    string name;
    vector<unique_ptr<Seat>> seats;

    void buildSeats() {
        seats.clear();
        for (int i = 0; i < capacity; ++i) {
            string num = string(1, char('A' + i / SEATS_PER_ROW)) + to_string(i % SEATS_PER_ROW + 1);
            seats.push_back(make_unique<Seat>(i + 1, num));
        }
    }

public:
    MovieHall(int hallId, int capacity, string name)
        : hallId(hallId), capacity(capacity), name(std::move(name)) {
        if (capacity < 1 || capacity > MAX_CAPACITY)
            throw InvalidInputException("Hall capacity must be between 1 and " + to_string(MAX_CAPACITY) + ".");
        buildSeats();
    }

    int getHallId() const { return hallId; }
    int getCapacity() const { return capacity; }
    string getName() const { return name; }
    const vector<unique_ptr<Seat>>& getSeats() const { return seats; }

    bool ownsSeat(const Seat* s) const {
        for (const auto& p : seats) if (p.get() == s) return true;
        return false;
    }
    Seat* findSeatById(int id) const {
        for (const auto& p : seats) if (p->getSeatId() == id) return p.get();
        throw NotFoundException("Seat");
    }
    Seat* findSeatByNumber(const string& num) const {
        string up = toUpper(num);
        for (const auto& p : seats) if (p->getSeatNumber() == up) return p.get();
        throw NotFoundException("Seat");
    }

    void display() const {
        cout << "[Hall ID: " << hallId << "] " << name << " (Capacity: " << capacity << " seats)\n";
    }

protected:
    friend class Admin;
    void setId(int v) { hallId = v; }
    void setCapacity(int v) {
        if (v < 1 || v > MAX_CAPACITY)
            throw InvalidInputException("Hall capacity must be between 1 and " + to_string(MAX_CAPACITY) + ".");
        capacity = v;
        buildSeats();
    }
    void setName(string v) { name = std::move(v); }
};

// ============================================================
// ShowTime
// ============================================================
class ShowTime {
    int showTimeId;
    string startTime;
    string endTime;
    Movie* movie;
    MovieHall* movieHall;
    map<int, bool> reservedSeats; // seatId -> reserved (per showtime)

public:
    ShowTime(int showTimeId, string startTime, string endTime, Movie* movie, MovieHall* movieHall)
        : showTimeId(showTimeId), startTime(std::move(startTime)), endTime(std::move(endTime)),
          movie(movie), movieHall(movieHall) {}

    int getShowTimeId() const { return showTimeId; }
    string getStartTime() const { return startTime; }
    string getEndTime() const { return endTime; }
    Movie* getMovie() const { return movie; }
    MovieHall* getMovieHall() const { return movieHall; }

    bool isSeatReserved(int seatId) const {
        auto it = reservedSeats.find(seatId);
        return it != reservedSeats.end() && it->second;
    }

    // Fix #5: a seat can only be reserved if it belongs to THIS showtime's hall.
    void reserveSeat(const Seat* seat) {
        if (!seat || !movieHall->ownsSeat(seat)) throw NotFoundException("Seat");
        if (isSeatReserved(seat->getSeatId())) throw SeatAlreadyReservedException();
        reservedSeats[seat->getSeatId()] = true;
    }

    int availableSeats() const {
        return movieHall->getCapacity() - (int)count_if(reservedSeats.begin(), reservedSeats.end(),
                                                       [](const pair<const int, bool>& p) { return p.second; });
    }

    void display() const {
        cout << "[Showtime ID: " << showTimeId << "] Movie: " << movie->getTitle()
             << " | Hall: " << movieHall->getName()
             << " | Time: " << startTime << " - " << endTime
             << " | Free seats: " << availableSeats() << "/" << movieHall->getCapacity() << "\n";
    }

    void displaySeatMap() const {
        cout << "\nSeat map for Showtime " << showTimeId << " (" << movieHall->getName()
             << ")   [A1] = available   [XX] = reserved\n";
        int col = 0;
        for (const auto& s : movieHall->getSeats()) {
            cout << (isSeatReserved(s->getSeatId()) ? "[XX]" : "[" + s->getSeatNumber() + "]");
            cout << string(s->getSeatNumber().size() == 2 ? 2 : 1, ' ');
            if (++col % MovieHall::SEATS_PER_ROW == 0) cout << "\n";
        }
        if (col % MovieHall::SEATS_PER_ROW != 0) cout << "\n";
    }

protected:
    friend class Admin;
    void setId(int v) { showTimeId = v; }
    void setStartTime(string v) { startTime = std::move(v); }
    void setEndTime(string v) { endTime = std::move(v); }
    void setMovie(Movie* v) { movie = v; }
    void setMovieHall(MovieHall* v) { movieHall = v; }
};

// ============================================================
// Ticket
// ============================================================
class Ticket {
    int ticketId;
    Seat* seat;
    MovieHall* movieHall;
    ShowTime* showTime;

public:
    Ticket(int ticketId, Seat* seat, MovieHall* movieHall, ShowTime* showTime)
        : ticketId(ticketId), seat(seat), movieHall(movieHall), showTime(showTime) {}

    int getTicketId() const { return ticketId; }
    Seat* getSeat() const { return seat; }
    MovieHall* getMovieHall() const { return movieHall; }
    ShowTime* getShowTime() const { return showTime; }

    void display() const {
        cout << "--- TICKET DETAILS ---\n"
             << "Ticket ID: " << ticketId << "\n"
             << "Movie: " << showTime->getMovie()->getTitle() << "\n"
             << "Hall: " << movieHall->getName() << "\n"
             << "Seat: " << seat->getSeatNumber() << "\n"
             << "Time: " << showTime->getStartTime() << " to " << showTime->getEndTime() << "\n"
             << "Price: $" << fixed << setprecision(2) << showTime->getMovie()->getPrice()
             << "\n----------------------\n";
    }

protected:
    friend class Admin;
    void setId(int v) { ticketId = v; }
    void setSeat(Seat* v) { seat = v; }
    void setMovieHall(MovieHall* v) { movieHall = v; }
    void setShowTime(ShowTime* v) { showTime = v; }
};

// ============================================================
// BookingDetails (now owns its Ticket - fix #8)
// ============================================================
class BookingDetails {
    int bookingId;
    unique_ptr<Ticket> ticket;

public:
    BookingDetails(int bookingId, unique_ptr<Ticket> ticket)
        : bookingId(bookingId), ticket(std::move(ticket)) {}

    int getBookingId() const { return bookingId; }
    Ticket* getTicket() const { return ticket.get(); }

    void display() const {
        cout << "Booking ID: " << bookingId << "\n";
        ticket->display();
    }
};

// ============================================================
// Admin  (Fix #2, #3, #4, #10)
// ============================================================
class Admin {
    int adminId;
    size_t passwordHash;

    // NOTE: salted std::hash removes plain-text storage but is NOT cryptographically
    // secure. A production system should use bcrypt/argon2 (a library dependency).
    static size_t hashPassword(int id, const string& pwd) {
        return hash<string>{}("cinema-salt:" + to_string(id) + ":" + pwd);
    }

public:
    Admin(int adminId, const string& password) : adminId(adminId) {
        if (adminId <= 0) throw InvalidInputException("Admin ID must be a positive number.");
        if (password.size() < 6) throw InvalidInputException("Password must be at least 6 characters.");
        passwordHash = hashPassword(adminId, password);
    }

    int getAdminId() const { return adminId; }
    bool authenticate(int id, const string& pwd) const {
        return adminId == id && passwordHash == hashPassword(id, pwd);
    }

    // ---- Additions ----
    Movie* addMovie(vector<unique_ptr<Movie>>& movies, int id, int duration, const string& date,
                    double price, int age, const string& title, const string& genre,
                    const string& lang, const string& desc) {
        for (const auto& m : movies) if (m->getId() == id) throw DuplicateIdException("Movie", id);
        validateMovieFields(id, duration, date, price, age, title, genre, lang);
        movies.push_back(make_unique<Movie>(id, duration, date, price, age, title, genre, lang,
                                            desc.empty() ? "No description available." : desc));
        cout << "Success: Movie added successfully!\n";
        return movies.back().get();
    }

    MovieHall* addMovieHall(vector<unique_ptr<MovieHall>>& halls, int id, int capacity, const string& name) {
        for (const auto& h : halls) {
            if (h->getHallId() == id) throw DuplicateIdException("Movie Hall", id);
            if (toLower(h->getName()) == toLower(name))
                throw InvalidInputException("A hall with that name already exists.");
        }
        if (id <= 0) throw InvalidInputException("Hall ID must be positive.");
        if (name.empty()) throw InvalidInputException("Hall name cannot be empty.");
        halls.push_back(make_unique<MovieHall>(id, capacity, name)); // validates capacity
        cout << "Success: Movie Hall added successfully!\n";
        return halls.back().get();
    }

    ShowTime* addShowTime(vector<unique_ptr<ShowTime>>& showtimes, int id, const string& start,
                          const string& end, Movie* movie, MovieHall* hall) {
        for (const auto& s : showtimes) if (s->getShowTimeId() == id) throw DuplicateIdException("Showtime", id);
        if (id <= 0) throw InvalidInputException("Showtime ID must be positive.");
        int s = parseTimeToMinutes(start), e = parseTimeToMinutes(end);
        if (e <= s) throw InvalidInputException("End time must be after start time.");
        if (e - s < movie->getDuration())
            throw InvalidInputException("Showtime is shorter than the movie (" +
                                        to_string(movie->getDuration()) + " mins).");
        for (const auto& st : showtimes) {
            if (st->getMovieHall() != hall) continue;
            int os = parseTimeToMinutes(st->getStartTime()), oe = parseTimeToMinutes(st->getEndTime());
            if (s < oe && os < e) throw ShowTimeConflictException();
        }
        showtimes.push_back(make_unique<ShowTime>(id, start, end, movie, hall));
        cout << "Success: Showtime scheduled successfully!\n";
        return showtimes.back().get();
    }

    // ---- Edits using the protected setters (fix #10) ----
    void editMovie(Movie& m, int duration, const string& date, double price, int age,
                   const string& title, const string& genre, const string& lang, const string& desc) {
        validateMovieFields(m.getId(), duration, date, price, age, title, genre, lang);
        m.setDuration(duration);
        m.setReleaseDate(date);
        m.setPrice(price);
        m.setAgeRating(age);
        m.setTitle(title);
        m.setGenre(genre);
        m.setLanguage(lang);
        m.setDescription(desc.empty() ? "No description available." : desc);
        cout << "Success: Movie updated successfully!\n";
    }

    void renameHall(const vector<unique_ptr<MovieHall>>& halls, MovieHall& hall, const string& name) {
        if (name.empty()) throw InvalidInputException("Hall name cannot be empty.");
        for (const auto& h : halls)
            if (h.get() != &hall && toLower(h->getName()) == toLower(name))
                throw InvalidInputException("A hall with that name already exists.");
        hall.setName(name);
        cout << "Success: Hall renamed successfully!\n";
    }

private:
    static void validateMovieFields(int id, int duration, const string& date, double price, int age,
                                    const string& title, const string& genre, const string& lang) {
        if (id <= 0) throw InvalidInputException("Movie ID must be positive.");
        if (duration < 1 || duration > 600) throw InvalidInputException("Duration must be 1-600 minutes.");
        validateDate(date);
        if (price < 0) throw InvalidInputException("Price cannot be negative.");
        if (age < 0 || age > 21) throw InvalidInputException("Age rating must be between 0 and 21.");
        if (title.empty() || genre.empty() || lang.empty())
            throw InvalidInputException("Title, genre and language cannot be empty.");
    }
};

// ============================================================
// Search functions
// ============================================================
vector<Movie*> searchMoviesByTitle(const vector<unique_ptr<Movie>>& movies, const string& query) {
    vector<Movie*> results;
    string q = toLower(query);
    for (const auto& m : movies)
        if (toLower(m->getTitle()).find(q) != string::npos) results.push_back(m.get());
    return results;
}

vector<Movie*> searchMoviesByGenre(const vector<unique_ptr<Movie>>& movies, const string& genreQuery) {
    vector<Movie*> results;
    string q = toLower(genreQuery);
    for (const auto& m : movies)
        if (toLower(m->getGenre()).find(q) != string::npos) results.push_back(m.get());
    return results;
}

MovieHall* searchMovieHallById(const vector<unique_ptr<MovieHall>>& halls, int hallId) {
    for (const auto& h : halls) if (h->getHallId() == hallId) return h.get();
    throw NotFoundException("Movie Hall");
}

MovieHall* searchMovieHallByName(const vector<unique_ptr<MovieHall>>& halls, const string& name) {
    for (const auto& h : halls) if (toLower(h->getName()) == toLower(name)) return h.get();
    throw NotFoundException("Movie Hall");
}

Movie* searchMovieById(const vector<unique_ptr<Movie>>& movies, int movieId) {
    for (const auto& m : movies) if (m->getId() == movieId) return m.get();
    throw NotFoundException("Movie");
}

ShowTime* searchShowTimeById(const vector<unique_ptr<ShowTime>>& showtimes, int id) {
    for (const auto& s : showtimes) if (s->getShowTimeId() == id) return s.get();
    throw NotFoundException("ShowTime");
}

// ============================================================
// Robust input helpers (fix #7)
// ============================================================
static string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return a == string::npos ? "" : s.substr(a, b - a + 1);
}

static string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) throw InputClosed();
    return trim(s);
}

static bool tryInt(const string& s, long lo, long hi, int& out) {
    try {
        size_t p = 0;
        long v = stol(s, &p);
        if (p != s.size() || v < lo || v > hi) return false;
        out = (int)v;
        return true;
    } catch (const exception&) { return false; }
}

static int readInt(const string& prompt, long lo = -1000000, long hi = 1000000) {
    while (true) {
        int v;
        if (tryInt(readLine(prompt), lo, hi, v)) return v;
        cout << "Error: please enter a whole number between " << lo << " and " << hi << ".\n";
    }
}

static bool tryDouble(const string& s, double& out) {
    try {
        size_t p = 0;
        double v = stod(s, &p);
        if (p != s.size() || v < 0 || v > 100000) return false;
        out = v;
        return true;
    } catch (const exception&) { return false; }
}

static double readDouble(const string& prompt) {
    while (true) {
        double v;
        if (tryDouble(readLine(prompt), v)) return v;
        cout << "Error: please enter a valid non-negative amount.\n";
    }
}

// ============================================================
// Application data
// ============================================================
struct Cinema {
    vector<unique_ptr<Movie>> movies;
    vector<unique_ptr<MovieHall>> halls;
    vector<unique_ptr<ShowTime>> showtimes;
    vector<unique_ptr<BookingDetails>> bookings;
    vector<unique_ptr<Admin>> admins;
    int ticketCounter = 1000;
    int bookingCounter = 5000;
};

static void seed(Cinema& c) {
    c.admins.push_back(make_unique<Admin>(1, "admin123"));
    c.movies.push_back(make_unique<Movie>(101, 148, "2010-07-16", 12.50, 13, "Inception", "Sci-Fi", "English",
                                          "A thief who steals corporate secrets..."));
    c.movies.push_back(make_unique<Movie>(102, 169, "2014-11-07", 14.00, 13, "Interstellar", "Sci-Fi", "English",
                                          "A team of explorers travel through a wormhole..."));
    c.halls.push_back(make_unique<MovieHall>(1, 5, "IMAX Hall"));
    c.halls.push_back(make_unique<MovieHall>(2, 4, "Standard Hall 1"));
    c.showtimes.push_back(make_unique<ShowTime>(1, "18:00", "20:30", c.movies[0].get(), c.halls[0].get()));
}

// ============================================================
// Admin portal
// ============================================================
static void adminSession(Cinema& c, Admin& admin) {
    while (true) {
        cout << "\nAdmin Menu:\n"
             << "1. Add Movie\n2. Edit Movie\n3. Add Movie Hall\n4. Rename Movie Hall\n"
             << "5. Schedule Showtime\n6. View All Movies\n7. View All Halls\n"
             << "8. View All Showtimes\n9. Logout\n";
        int act = readInt("Enter choice: ", 1, 9);
        try {
            if (act == 1) {
                int id = readInt("Movie ID: ", 1);
                int dur = readInt("Duration (mins): ", 1, 600);
                string date = readLine("Release Date (YYYY-MM-DD): ");
                double price = readDouble("Price ($): ");
                int age = readInt("Age Rating (+): ", 0, 21);
                string title = readLine("Title: ");
                string genre = readLine("Genre: ");
                string lang = readLine("Language: ");
                string desc = readLine("Description: ");
                admin.addMovie(c.movies, id, dur, date, price, age, title, genre, lang, desc);
            } else if (act == 2) {
                Movie* m = searchMovieById(c.movies, readInt("Movie ID to edit: ", 1));
                m->display();
                cout << "(Press Enter to keep the current value)\n";
                int dur = m->getDuration(); double price = m->getPrice(); int age = m->getAgeRating();
                string s = readLine("Duration [" + to_string(m->getDuration()) + "]: ");
                if (!s.empty() && !tryInt(s, 1, 600, dur)) throw InvalidInputException("Invalid duration.");
                string date = readLine("Release Date [" + m->getReleaseDate() + "]: ");
                if (date.empty()) date = m->getReleaseDate();
                s = readLine("Price [" + to_string(m->getPrice()) + "]: ");
                if (!s.empty() && !tryDouble(s, price)) throw InvalidInputException("Invalid price.");
                s = readLine("Age Rating [" + to_string(m->getAgeRating()) + "]: ");
                if (!s.empty() && !tryInt(s, 0, 21, age)) throw InvalidInputException("Invalid age rating.");
                string title = readLine("Title [" + m->getTitle() + "]: ");
                if (title.empty()) title = m->getTitle();
                string genre = readLine("Genre [" + m->getGenre() + "]: ");
                if (genre.empty()) genre = m->getGenre();
                string lang = readLine("Language [" + m->getLanguage() + "]: ");
                if (lang.empty()) lang = m->getLanguage();
                string desc = readLine("Description (Enter = keep): ");
                if (desc.empty()) desc = m->getDescription();
                admin.editMovie(*m, dur, date, price, age, title, genre, lang, desc);
            } else if (act == 3) {
                int id = readInt("Hall ID: ", 1);
                int cap = readInt("Capacity: ", 1, MovieHall::MAX_CAPACITY);
                string name = readLine("Hall Name: ");
                admin.addMovieHall(c.halls, id, cap, name);
            } else if (act == 4) {
                MovieHall* h = searchMovieHallById(c.halls, readInt("Hall ID to rename: ", 1));
                admin.renameHall(c.halls, *h, readLine("New name: "));
            } else if (act == 5) {
                int stId = readInt("Showtime ID: ", 1);
                int mId = readInt("Movie ID: ", 1);
                int hId = readInt("Hall ID: ", 1);
                string start = readLine("Start Time (HH:MM): ");
                string end = readLine("End Time (HH:MM): ");
                Movie* m = searchMovieById(c.movies, mId);
                MovieHall* h = searchMovieHallById(c.halls, hId);
                admin.addShowTime(c.showtimes, stId, start, end, m, h);
            } else if (act == 6) {
                cout << "\n--- Current Movies ---\n";
                for (auto& m : c.movies) m->display();
            } else if (act == 7) {
                cout << "\n--- Current Halls ---\n";
                for (auto& h : c.halls) h->display();
            } else if (act == 8) {
                cout << "\n--- Current Showtimes ---\n";
                if (c.showtimes.empty()) cout << "None scheduled.\n";
                for (auto& s : c.showtimes) s->display();
            } else {
                cout << "Logged out successfully.\n";
                return;
            }
        } catch (const CinemaException& e) {
            cout << e.what() << "\n";
        }
    }
}

static void adminPortal(Cinema& c) {
    cout << "\n--- Admin Portal ---\n1. Sign In\n2. Sign Up (Register New Admin)\n3. Back\n";
    int ch = readInt("Enter choice: ", 1, 3);
    try {
        if (ch == 1) {
            int id = readInt("Enter Admin ID: ");
            string pwd = readLine("Enter Password: ");
            Admin* logged = nullptr;
            for (auto& a : c.admins) if (a->authenticate(id, pwd)) { logged = a.get(); break; }
            if (!logged) throw InvalidCredentialsException();
            cout << "\nLogin Successful! Welcome Admin #" << id << "\n";
            adminSession(c, *logged);
        } else if (ch == 2) {
            int id = readInt("Choose Admin ID: ", 1);
            string pwd = readLine("Choose Password (min 6 chars): ");
            for (auto& a : c.admins) if (a->getAdminId() == id) throw AdminAlreadyExistsException();
            c.admins.push_back(make_unique<Admin>(id, pwd));
            cout << "Admin account created successfully! Please log in.\n";
        }
    } catch (const CinemaException& e) {
        cout << e.what() << "\n";
    }
}

// ============================================================
// Customer portal
// ============================================================
static void printMovies(const vector<Movie*>& v, const string& emptyMsg) {
    if (v.empty()) { cout << emptyMsg << "\n"; return; }
    cout << "\nResults:\n";
    for (auto m : v) m->display();
}

static void bookTicket(Cinema& c) {
    if (c.showtimes.empty()) { cout << "No showtimes available to book.\n"; return; }
    for (auto& s : c.showtimes) s->display();
    ShowTime* show = searchShowTimeById(c.showtimes, readInt("Enter Showtime ID: ", 1));
    int age = readInt("Enter your age: ", 1, 120);
    if (age < show->getMovie()->getAgeRating()) throw AgeRestrictedException(show->getMovie()->getAgeRating());

    MovieHall* hall = show->getMovieHall();
    if (show->availableSeats() == 0) throw InvalidInputException("This showtime is sold out.");
    show->displaySeatMap();
    Seat* seat = hall->findSeatByNumber(readLine("Enter seat number to book (e.g. A3): "));

    show->reserveSeat(seat); // throws if already reserved
    auto ticket = make_unique<Ticket>(c.ticketCounter++, seat, hall, show);
    c.bookings.push_back(make_unique<BookingDetails>(c.bookingCounter++, std::move(ticket)));
    cout << "\nBooking Confirmed Successfully!\n";
    c.bookings.back()->display();
}

static void customerPortal(Cinema& c) {
    while (true) {
        cout << "\n--- Customer Portal ---\n"
             << "1. Browse All Movies\n2. Search Movies by Title\n3. Search Movies by Genre\n"
             << "4. View All Showtimes\n5. View Movie Details\n6. View Seat Availability\n"
             << "7. Book a Ticket\n8. Return to Main Menu\n";
        int ch = readInt("Enter choice: ", 1, 8);
        try {
            if (ch == 1) {
                vector<Movie*> all;
                for (auto& m : c.movies) all.push_back(m.get());
                printMovies(all, "No movies available.");
            } else if (ch == 2) {
                printMovies(searchMoviesByTitle(c.movies, readLine("Enter title keyword: ")),
                            "No movies found matching that title.");
            } else if (ch == 3) {
                printMovies(searchMoviesByGenre(c.movies, readLine("Enter genre: ")),
                            "No movies found in that genre.");
            } else if (ch == 4) {
                cout << "\n--- Available Showtimes ---\n";
                if (c.showtimes.empty()) cout << "No showtimes available at the moment.\n";
                for (auto& s : c.showtimes) s->display();
            } else if (ch == 5) {
                Movie* m = searchMovieById(c.movies, readInt("Enter Movie ID: ", 1));
                m->displayDetails();
                cout << "Showtimes:\n";
                bool any = false;
                for (auto& s : c.showtimes)
                    if (s->getMovie() == m) { s->display(); any = true; }
                if (!any) cout << "  (none scheduled)\n";
            } else if (ch == 6) {
                if (c.showtimes.empty()) { cout << "No showtimes available.\n"; continue; }
                for (auto& s : c.showtimes) s->display();
                searchShowTimeById(c.showtimes, readInt("Enter Showtime ID: ", 1))->displaySeatMap();
            } else if (ch == 7) {
                cout << "\n--- Book a Ticket ---\n";
                bookTicket(c);
            } else {
                return;
            }
        } catch (const CinemaException& e) {
            cout << e.what() << "\n";
        }
    }
}

// ============================================================
// main
// ============================================================
int main() {
    Cinema cinema;
    seed(cinema);

    cout << "=========================================\n"
         << "   WELCOME TO CINEMA MANAGEMENT SYSTEM   \n"
         << "=========================================\n";
    try {
        while (true) {
            cout << "\nSelect Role:\n1. Admin\n2. Customer\n3. Exit\n";
            int choice = readInt("Enter choice: ", 1, 3);
            if (choice == 1) adminPortal(cinema);
            else if (choice == 2) customerPortal(cinema);
            else break;
        }
        cout << "Thank you for using the Cinema Management System. Goodbye!\n";
    } catch (const InputClosed&) {
        cout << "\nInput closed. Exiting.\n";
    }
    return 0; // all memory released automatically via smart pointers
}
