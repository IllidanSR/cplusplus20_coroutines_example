#include <coroutine>
#include <memory>
#include <iostream>


template<typename T>
struct generator {
    struct promise_type {
        promise_type() {
            std::cout << "Promise created" << std::endl;
        }
        ~promise_type() {
            std::cout << "Promise died" << std::endl;
        }
        auto initial_suspend() {
            std::cout << "Started the coroutine, let's pause!" << std::endl;
            return std::suspend_never{};
        }
        auto final_suspend() {
            std::cout << "Finished the coroutine" << std::endl;
            return std::suspend_always{};
        }
        auto get_return_object() {
            std::cout << "Send back a generator" << std::endl;
            return generator{handle_type::from_promise(*this)};
        }
        void unhandled_exception() {
            std::cout << "Exception...." << std::endl;
            std::exit(1);
        }
        auto return_void() {
            std::cout << "return_void" << std::endl;
            return std::suspend_never{};
        }
        auto yield_value(T value) {
            std::cout << "Yielded " << value << std::endl;
            current_value = value;
            return std::suspend_always{};
        }

        T current_value;

    };
    using handle_type =
    std::coroutine_handle<promise_type>;

    T current_value() {
        return coro.promise().current_value;
    }

    bool move_next() {
        std::cout << "Moving to next" << std::endl;;
        coro.resume();
        std::cout << "Are we done? ";
        auto still_going = not coro.done();
        std::cout << (still_going ? "There is another" : "We're done") << std::endl;
        return still_going;
    }
    ~generator() {
        std::cout << "~generator "
                  << (not coro ? "(empty)" : "(contains a coroutine)")
                  << std::endl;
        if ( coro ) coro.destroy();
    }
    generator(const generator &) = delete;
    generator(generator &&g)
            : coro(g.coro) {
        g.coro = nullptr;
    };


private:
    generator(handle_type h)
            : coro(h) {
        std::cout << "generator" << std::endl;
    }
    handle_type coro;
};



generator<std::string> count() {
    std::cout << "Going to yield 1" << std::endl;
    std::string v = "VALUE";
    co_yield v;
    std::cout << "Going to yield 2" << std::endl;
    co_yield "V2";
    std::cout << "Going to yield 3" << std::endl;
    co_yield "V3";
    std::cout << "count() is done" << std::endl;
}

int main() {
    auto g = count();
    while ( g.move_next() )
        std::cout << ">> " << g.current_value() << std::endl;
    return 0;
}
