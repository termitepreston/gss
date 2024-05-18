
#include <optional>

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <exception>
#include <fmt/core.h>
#include <string>

#include <spdlog/spdlog.h>

#include "listener.hpp"
#include "shared_state.hpp"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>

// This file will be generated automatically when cur_you run the CMake
// configuration step. It creates a namespace called `gss`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>

int main(int argc, const char **argv) {
    try {
        CLI::App app{fmt::format("{} version {}", gss::cmake::project_name,
                                 gss::cmake::project_version)};

        std::optional<unsigned short> port;
        std::optional<int> threads_in;

        app.add_option("-p, --port", port, "Port to run the server on.");

        std::optional<std::string> address_in;
        app.add_option("-a,--address", address_in,
                       "Address to bind the server to.");

        app.add_option("-t, --threads", threads_in,
                       "Number of threads to run the server on.");

        bool show_version = false;
        app.add_flag("--version", show_version, "Show version information");

        CLI11_PARSE(app, argc, argv);

        if (show_version) {
            fmt::print("{}\n", gss::cmake::project_version);
            return EXIT_SUCCESS;
        }

        auto const address =
            net::ip::make_address(address_in.value_or("0.0.0.0"));

        const char *doc_root = "/home/alazar/dev/gss/src/gss-server";
        const auto threads = threads_in.value_or(1);

        net::io_context ioc;

        boost::make_shared<listener>(ioc, tcp::endpoint{address, port.value()},
                                     boost::make_shared<shared_state>(doc_root))
            ->run();

        net::signal_set signals{ioc, SIGINT, SIGTERM};

        signals.async_wait(
            [&ioc](const boost::system::error_code &, int) { ioc.stop(); });

        std::vector<std::thread> v;

        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i)
            v.emplace_back([&ioc] { ioc.run(); });

        ioc.run();

        spdlog::info("running server on {}:{}.", address.to_string(),
                     port.value());

        for (auto &t : v)
            t.join();

    } catch (const std::exception &e) {
        spdlog::error("Unhandled exception in main: {}", e.what());
    }
}
