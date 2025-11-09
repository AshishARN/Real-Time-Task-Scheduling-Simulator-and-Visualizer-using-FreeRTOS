import matplotlib.pyplot as plt
import re

def parse_log(log_file):
    """ Parses the QEMU log file to extract scheduler trace events. """
    events = []
    # Regex to find lines like "TICK,TASK_NAME,IN"
    trace_pattern = re.compile(r'^(\d+),([^,]+),(IN)$')

    with open(log_file, 'r') as f:
        for line in f:
            match = trace_pattern.match(line.strip())
            if match:
                tick = int(match.group(1))
                task_name = match.group(2)
                events.append({'tick': tick, 'task': task_name})
    return events

def generate_gantt_chart(events):
    """ Generates and displays a Gantt chart from the parsed events. """
    if not events:
        print("No trace events found. Is the log file correct?")
        return

    fig, ax = plt.subplots(figsize=(15, 6))

    tasks = sorted(list(set(e['task'] for e in events)), reverse=True)
    task_y_pos = {task: i for i, task in enumerate(tasks)}

    for i in range(len(events) - 1):
        start_event = events[i]
        end_event = events[i+1]
        
        task_name = start_event['task']
        start_time = start_event['tick']
        duration = end_event['tick'] - start_time
        y_pos = task_y_pos[task_name]

        if duration > 0:
            ax.broken_barh([(start_time, duration)], (y_pos - 0.4, 0.8),
                           facecolors=('tab:blue' if 'H' in task_name else
                                       'tab:orange' if 'M' in task_name else
                                       'tab:green' if 'L' in task_name else 'tab:grey'))

    ax.set_yticks([i for i in range(len(tasks))])
    ax.set_yticklabels(tasks)
    ax.set_xlabel('Time (System Ticks)')
    ax.set_ylabel('Task')
    ax.set_title('Real-Time Task Execution Timeline (Priority Inversion Solved)')
    ax.grid(True)
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    log_file = 'trace.log'
    parsed_events = parse_log(log_file)
    generate_gantt_chart(parsed_events)