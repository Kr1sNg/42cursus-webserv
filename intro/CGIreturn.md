This is a classic and critical part of building a web server. Your `webserv` (the parent process) must be a strict supervisor over the CGI scripts (the child processes) it spawns.

The best way to do this is by using **timeouts** (for infinite loops) and **monitoring exit codes** (for errors).

Here’s how you can implement this in your server.

-----

### 1\. ⏱️ Handling Infinite Loops (Timeouts)

An infinite loop is just a script that runs for too long. Your server can't *know* it's an infinite loop, but it can—and must—decide a "reasonable" time limit.

The solution is to have your main server process check a timer. This is best done in your main I/O loop (where you use `poll()`, `select()`, `kqueue()`, etc.).

**Implementation Steps:**

1.  **Store a Start Time:** When your parent `forks()` to create the CGI process, store the `child_pid` and the current time.

    ```cpp
    // In your parent process, after fork()
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // ... child process: dup2() and execve() ...
    }

    // Parent process:
    // Store this info in a struct associated with your connection
    CGI_Job job;
    job.pid = child_pid;
    job.startTime = time(NULL); 
    ```

2.  **Check in Your Main Loop:** In your server's main event loop, you are already checking sockets. Add a check for your running CGI jobs.

    ```cpp
    // In your main server loop
    while (true) {
        // ... poll/select on network sockets ...

        // Also, loop through all active CGI jobs
        for (CGI_& job : active_cgi_jobs) {

            // 1. Check for timeout
            if (time(NULL) - .startTime > CGI_TIMEOUT_SECONDS) {
                // Script took too long!
                kill(job.pid, SIGKILL); // Forcefully kill the child
                
                // ... clean up the job, close pipes ...
                // Send a 504 Gateway Timeout to the client
                send_http_response(client_fd, 504); 
                continue;
            }

            // 2. Check if it finished (see next section)
        }
    }
    ```

3.  **Use `waitpid()` with `WNOHANG`:** You must use `waitpid()` to "reap" the child process when it exits. A non-blocking `waitpid()` is essential so your server doesn't freeze.

    ```cpp
    // Inside your loop (either just after the timeout check or in a separate loop)
    int status;
    pid_t finished_pid = waitpid(job.pid, &status, WNOHANG);

    if (finished_pid == job.pid) {
        // The process *just* finished. 
        // Now you can check *how* it finished.
        handle_cgi_exit(job, status); 
    }
    // If finished_pid == 0, it's still running (and we continue)
    // If finished_pid == -1, there was an error
    ```

-----

### 2\. ❌ Handling Script Errors (Exit Codes & `stderr`)

A script can fail in two ways: it can crash (e.g., segmentation fault) or it can exit intentionally with a non-zero status.

#### A. Checking Exit Status

The `status` variable from `waitpid()` tells you everything. You **must** use the macros to interpret it.

```cpp
void handle_cgi_exit(CGI_Job& job, int status) {
    
    if (WIFEXITED(status)) {
        // Case 1: The script exited normally.
        int exit_code = WEXITSTATUS(status);
        
        if (exit_code == 0) {
            // SUCCESS!
            // The script ran and finished without error.
            // You're done. Send 200 OK (or whatever CGI header said).
        } else {
            // SCRIPT ERROR!
            // The script (py, bash, etc.) failed and returned a non-zero code.
            // This is a 500 Internal Server Error.
            send_http_response(job.client_fd, 500);
        }

    } else if (WIFSIGNALED(status)) {
        // Case 2: The script was killed by a signal.
        int signal_number = WTERMSIG(status);
        
        // This could be from a crash (like SIGSEGV)
        // or from *our own timeout* (SIGKILL).
        
        // This is also a 500 Internal Server Error
        // (unless it was our timeout, which we already handled).
        send_http_response(job.client_fd, 500);
    }
    
    // ... clean up the job, close pipes ...
}
```

#### B. Capturing `stderr` for Logging

To debug *why* a script failed (e.g., Python `SyntaxError`), you must read from its `stderr`.

When you set up the CGI, you need **two** pipes:

1.  **`stdout_pipe`**: Connects child's `STDOUT_FILENO` to your server. This data is sent to the web client.
2.  **`stderr_pipe`**: Connects child's `STDERR_FILENO` to your server. This data is **logged to your server's error log**.

**In the Child Process (after `fork`):**

```cpp
// child_pid == 0
dup2(stdout_pipe[1], STDOUT_FILENO); // Reroute stdout
dup2(stderr_pipe[1], STDERR_FILENO); // Reroute stderr
close(stdout_pipe[0]);
close(stderr_pipe[0]);
// ...
execve(...);
```

**In the Parent Process:**

  * Add the read-ends (`stdout_pipe[0]` and `stderr_pipe[0]`) to your `poll()`/`select()`/`kqueue()` set.
  * When `stdout_pipe[0]` has data, read it and send it to the client.
  * When `stderr_pipe[0]` has data, read it and write it to your `webserv.log` file.

This way, when a Python script fails, the traceback error is written to `stderr`, your server captures it, and you can see the exact bug in your logs.