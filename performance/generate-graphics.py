import matplotlib.pyplot as plt
from matplotlib.pyplot import clf

FILE_NAME = 'measures.txt'
INSERT_TIME = 'plots/insert_time.png'
INSERT_READ = 'plots/insert_read.png'
INSERT_WRITE = 'plots/insert_write.png'
SEARCH_TIME = 'plots/search_time.png'
SEARCH_READ = 'plots/search_read.png'

N_ITERATIONS = 3

x = []
insert_time_y = []
insert_read_y = []
insert_write_y = []
search_time_y = []
search_read_y = []

with open(FILE_NAME) as file:
    for line in file:
        row = list(map(int, line.strip().split(' ')))
        n = row[0]
        insert_time = row[1]
        insert_read = row[2]
        insert_write = row[3]
        search_time = row[4]
        search_read = row[5]
        search_write = row[6]
        if (len(x) == 0 or x[-1] != n):
            x += [n]
            insert_time_y += [insert_time]
            insert_read_y += [insert_read]
            insert_write_y += [insert_write]
            search_time_y += [search_time]
            search_read_y += [search_read]
        else:
            insert_time_y[-1] += insert_time
            insert_read_y[-1] += insert_read
            insert_write_y[-1] += insert_write
            search_time_y[-1] += search_time
            search_read_y[-1] += search_read

for i in range(len(x)):
    insert_time_y[i] = 1.0 * insert_time_y[i] / N_ITERATIONS
    insert_read_y[i] = 1.0 * insert_read_y[i] / N_ITERATIONS
    insert_write_y[i] = 1.0 * insert_write_y[i] / N_ITERATIONS
    search_time_y[i] = 1.0 * search_time_y[i] / N_ITERATIONS
    search_read_y[i] = 1.0 * search_read_y[i] / N_ITERATIONS

# Insert time
plt.plot(x, insert_time_y, color = 'blue')
plt.xlabel('n: number of elements')
plt.ylabel('t (ms): time')
plt.legend(['Insert time'])
plt.savefig(INSERT_TIME)
clf()

# Insert reads
plt.plot(x, insert_read_y, color = 'blue')
plt.xlabel('n: number of elements')
plt.ylabel('Disk queries')
plt.legend(['Insert number reads'])
plt.savefig(INSERT_READ)
clf()

# Insert writes
plt.plot(x, insert_write_y, color = 'blue')
plt.xlabel('n: number of elements')
plt.ylabel('Disk queries')
plt.legend(['Insert number writes'])
plt.savefig(INSERT_WRITE)
clf()

# Search time
plt.plot(x, search_time_y, color = 'blue')
plt.xlabel('n: number of elements')
plt.ylabel('t (ms): time')
plt.legend(['Search time'])
plt.savefig(SEARCH_TIME)
clf()

# Search reads
plt.plot(x, search_read_y, color = 'blue')
plt.xlabel('n: number of elements')
plt.ylabel('Disk queries')
plt.legend(['Search number reads'])
plt.savefig(SEARCH_READ)
clf()
