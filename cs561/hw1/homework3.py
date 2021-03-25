import heapq
import math
import queue as Q
import sys
sys.setrecursionlimit(10**6)


def move(position, direction):
	x = position[0]
	y = position[1]
	z = position[2]
	if direction == 0: return (x, y, z)
	elif direction == 1: return (x+1, y, z)
	elif direction == 2: return (x-1, y, z)
	elif direction == 3: return (x, y+1, z)
	elif direction == 4: return (x, y-1, z)
	elif direction == 5: return (x, y, z+1)
	elif direction == 6: return (x, y, z-1)
	elif direction == 7: return (x+1, y+1, z)
	elif direction == 8: return (x+1, y-1, z)
	elif direction == 9: return (x-1, y+1, z)
	elif direction == 10: return (x-1, y-1, z)
	elif direction == 11: return (x+1, y, z+1)
	elif direction == 12: return (x+1, y, z-1)
	elif direction == 13: return (x-1, y, z+1)
	elif direction == 14: return (x-1, y, z-1)
	elif direction == 15: return (x, y+1, z+1)
	elif direction == 16: return (x, y+1, z-1)
	elif direction == 17: return (x, y-1, z+1)
	elif direction == 18: return (x, y-1, z-1)
	else: raise ValueError


def recede(position, direction):
	x = position[0]
	y = position[1]
	z = position[2]
	if direction == 0: return (x, y, z)
	elif direction == 1: return (x-1, y, z)
	elif direction == 2: return (x+1, y, z)
	elif direction == 3: return (x, y-1, z)
	elif direction == 4: return (x, y+1, z)
	elif direction == 5: return (x, y, z-1)
	elif direction == 6: return (x, y, z+1)
	elif direction == 7: return (x-1, y-1, z)
	elif direction == 8: return (x-1, y+1, z)
	elif direction == 9: return (x+1, y-1, z)
	elif direction == 10: return (x+1, y+1, z)
	elif direction == 11: return (x-1, y, z-1)
	elif direction == 12: return (x-1, y, z+1)
	elif direction == 13: return (x+1, y, z-1)
	elif direction == 14: return (x+1, y, z+1)
	elif direction == 15: return (x, y-1, z-1)
	elif direction == 16: return (x, y-1, z+1)
	elif direction == 17: return (x, y+1, z-1)
	elif direction == 18: return (x, y+1, z+1)
	else: raise ValueError


def outBorder(position, size):
	if position[0] >= size[0] or position[1] >= size[1] or position[2] >= size[2]:
		return True
	return False


def BFS(begin, end, graph, size):
	if begin == end:
		return writeOutput(0, 1, end, 0, visited, 0)

	queue = [(begin, 0)]	# only save last point & last move direction
	visited = {}
	visited[begin] = 0

	while len(queue) > 0:
		(lastPoint, lastMove) = queue.pop(0)
		if lastPoint in graph:
			operations = graph[lastPoint]
			for op in operations:
				newPoint = move(lastPoint, op)
				if newPoint in visited or outBorder(newPoint, size):
					continue
				visited[newPoint] = op
				if newPoint == end:
					totalCost, step = getPath(end, op, visited, 1)
					return writeOutput(totalCost, step, end, op, visited, 1)
				queue.append((newPoint, op))
	return fail()


def mysort(e):
	return e[0]


def UCS(begin, end, graph, size):
	if begin == end:
		return writeOutput(0, 1, end, 0, visited, 0)

	queue = [(0, (begin, 0))]	# only save last point & last move direction
	visited = {}
	visited[begin] = 0

	while len(queue) > 0:
		queue.sort(key=mysort)
		(totalCost, last) = queue.pop(0)
		(lastPoint, lastMove) = last
		if lastPoint in graph:
			operations = graph[lastPoint]
			for op in operations:
				newPoint = move(lastPoint, op)
				if newPoint in visited or outBorder(newPoint, size):
					continue
				visited[newPoint] = op
				cost = 10 if op < 7 else 14
				if newPoint == end:
					totalCost, step = getPath(end, op, visited, 0)
					return writeOutput(totalCost, step, end, op, visited, 0)
				queue.append((totalCost + cost, (newPoint, op)))
	return fail()


def heuristic(point, goal):
	return int((abs(point[0] - goal[0]) + abs(point[1] - goal[1]) + abs(point[2] - goal[2])) / 3)


def Astar(begin, end, graph, size):
	if begin == end:
		return writeOutput(0, 1, end, 0, visited, 0)

	queue = [(0, (begin, 0))]	# only save last point & last move direction
	visited = {}
	visited[begin] = 0

	while len(queue) > 0:
		queue.sort(key=mysort)
		(totalCost, last) = queue.pop(0)
		(lastPoint, lastMove) = last
		if lastPoint in graph:
			operations = graph[lastPoint]
			for op in operations:
				newPoint = move(lastPoint, op)
				if newPoint in visited or outBorder(newPoint, size):
					continue
				visited[newPoint] = op
				cost = 10 if op < 7 else 14
				if newPoint == end:
					totalCost, step = getPath(end, op, visited, 0)
					return writeOutput(totalCost, step, end, op, visited, 0)
				queue.append((totalCost + cost + heuristic(newPoint, end), (newPoint, op)))
	return fail()


def getPath(point, move, visited, BFS):
	totalCost = 0
	step = 1
	while move != 0:
		step += 1
		cost = 1 if BFS else (10 if move < 7 else 14)
		totalCost += cost
		point = recede(point, move)
		move = visited[point]
		if move == 0:		# current point is begin point
			return totalCost, step
		
	return totalCost, step


def mainProcess():
	f = open('input6.txt' , 'r')
	method = f.readline().strip()

	dimensions = list(map(int, f.readline().strip().split()))
	entrance = tuple(map(int, f.readline().strip().split())) 
	exit = tuple(map(int, f.readline().strip().split()))

	Graph = {}
	num = int(f.readline().strip())
	for i in range(num):
		grid = list(map(int, f.readline().strip().split())) 
		point = tuple(grid[:3])
		operations = grid[3:]
		operations.sort()
		Graph[point] = operations
	if method == 'BFS':
		return BFS(exit, entrance, Graph, dimensions)
	elif method == 'UCS':
		return UCS(exit, entrance, Graph, dimensions)
	elif method == 'A*':
		return Astar(exit, entrance, Graph, dimensions)
	else:
		raise ValueError


def writeOutput(totalCost, step, point, move, visited, BFS):
	f = open('output.txt', 'w')
	f.write(str(totalCost) + '\n')
	f.write(str(step))
	f.write("\n{0} {1} {2} 0".format(point[0], point[1], point[2]))

	while move != 0:
		point = recede(point, move)
		cost = 1 if BFS else (10 if move < 7 else 14)
		f.write("\n{0} {1} {2} {3}".format(point[0], point[1], point[2], cost))
		move = visited[point]
	
	return


def fail():
	f = open('output.txt', 'w')
	f.write('FAIL')
	return


if __name__ == "__main__":
	mainProcess()


