import pygame
from pygame.locals import *
from sys import exit
import time

SCREEN_SIZE = (640,600)
SCREEN_WIDTH = 640
SCREEN_HEIGHT = 600
PLAYER_DEFAULT_POSITION = (270,515) #Player will always start or restart on this position
FIRST_ENEMY_DEFAULT_POSITION = (45,30)
FIRST_ENEMY_DEFAULT_X = 45
NUMBER_SHIPS_ROW = 9
RIGHT_BORDER_LIMIT = 570
LEFT_BORDER_LIMIT = 5
Y_AXIS_MOVEMENT = 20

#This class is responsible for managing the elements on screen and being the screen itself

class Node:

    #Initializing the Node class

    def __init__(self, value=None, next_node=None):

        self.value = value
        self.next = next_node

    #String representation of the Node class

    def __repr__(self):
        return "%s -> %s" % (self.value,self.next)

class LinkedList:

    #Initializing the parameters

    def __init__(self):
        self.head = None

    #String representation of the Linked List class

    def __repr__(self):
        return "%s" % self.head

    #Add a new element to the end of a Linked List

    def add(self,ship):

        Aux_Node = Node(ship)

        if self.head is None:

            self.head = Aux_Node

        else:

            Aux_List = LinkedList()
            Aux_List.head = self.head

            while Aux_List.head.next is not None:

                Aux_List.head = Aux_List.head.next

            Aux_List.head.next = Aux_Node

    def delete(self,ship):

        if self.head is not None:

            Aux_Node = Node()
            Aux_Node.next = self.head

            Aux_List = LinkedList()
            Aux_List.head = self.head.next

            while Aux_List is not None:

                if self.head.value == ship and Aux_Node.next == self.head:

                    self.head = self.head.next
                    Aux_Node.next = self.head
                    break

                if Aux_List.head.value == ship:

                    self.head.next = Aux_List.head.next
                    break

                self.head = self.head.next
                Aux_List.head = Aux_List.head.next

            self.head = Aux_Node.next

        else:

            print("Empty List")

class Space:

    #Initializing the Space class

    def __init__(self):

        self.ships = LinkedList()
        background_file_name = r'.\images\background\background.png'
        self.background = pygame.image.load(background_file_name).convert()
        self.score = 0
        self.score_position = (30,10)
        self.lives = 1
        self.lives_position = (550,10)
        self.stage = 1

    def show_text(self,font):

        score_surface = font.render("Score: %s" % self.score, True, (255, 255, 255))
        screen.blit(score_surface, self.score_position)

        lives_surface = font.render("Lives: %s" % self.lives, True, (255, 255, 255))
        screen.blit(lives_surface, self.lives_position)

#All of the game objects (the player ship, enemy ships, obstacles and bonus ship) will share some universal features

class GameObject:

    #Initializing the GameObject class

    def __init__(self,id=None,screen_pos=None,image=None):

        self.id = id
        self.screen_pos = screen_pos
        self.image = image
        self.path = ".\images\g_obj" + str(self.id)

    #String representation of the GameObject class

    def __repr__(self):

        return "id: %s x,y: %s image: %s" % (self.id,self.screen_pos,self.image)

    #Atribute the name of the image file of the ship and it's path to a string

    def create_image(self,file_name):

        game_object_file_name = self.path + file_name + ".png"
        self.image = pygame.image.load(game_object_file_name).convert_alpha()

#Class for the game ships, including the player's.

class Player_Ship(GameObject):

    #Initializing the Ship class

    def __init__(self,id,screen_pos,image=None,width=100,height=100,speed=100):
        super().__init__(id,screen_pos,image)
        self.width = width
        self.height = height
        self.speed = speed

    def move_player_ship(self,time_function):

        distance = self.speed * time_function
        new_x = self.screen_pos[0] + distance

        if (new_x + self.width/1.5) <= SCREEN_WIDTH and new_x > -35:

            self.screen_pos = (new_x,self.screen_pos[1])

    def player_shoot(self):

        Bullet = Player_Bullet(self.id,(self.screen_pos[0]+self.width/2.33,self.screen_pos[1]))
        Bullet.create_image(r'\bullet1')
        screen.blit(Bullet.image, Bullet.screen_pos)

        return Bullet

class Player_Bullet(GameObject):

    def __init__(self, id, screen_pos, image=None,speed=250):
        super().__init__(id,screen_pos,image)
        self.speed = speed

    def __repr__(self):

        return "(%s,%s) / %s" % (self.screen_pos[0],self.screen_pos[1],self.image)

    def move_bullet(self,time_elapsed):

        distance = self.speed * time_elapsed
        new_y = self.screen_pos[1] - distance
        self.screen_pos = (self.screen_pos[0],new_y)

class Enemy_Ship(GameObject):

    def __init__(self,id,screen_pos,image=None,width=60,height=60,speed=225):
        super().__init__(id,screen_pos,image)
        self.width = width
        self.height = height
        self.speed = speed

    def move_enemy_ship(self,time_elapsed):

        distance = self.speed * time_elapsed
        new_x = self.screen_pos[0] + distance

        if (new_x + self.width) <= SCREEN_WIDTH:

            self.screen_pos = (new_x, self.screen_pos[1])

#function that will copy on the screen all units on its starting positions and fill the GameSpace dict with its x,y pos
def game_start():

    Game = Space()

    Player = Player_Ship(1,PLAYER_DEFAULT_POSITION)
    Player.create_image("\g_obj1")
    Game.ships.add(Player)
    id = 6 #Player ship has id 1. Enemy ships range from 2 to 6
    ships_count = 0
    x_ship_position, y_ship_position = FIRST_ENEMY_DEFAULT_POSITION

    while id > 1: #It's done backwards because the stronger ships are the ones at the top of the screen

        ship_position = (x_ship_position,y_ship_position)
        Enemy = Enemy_Ship(id,ship_position)
        Enemy.create_image("\g_obj"+str(id))
        Game.ships.add(Enemy)
        ships_count += 1
        x_ship_position += Enemy.width

        if ships_count == NUMBER_SHIPS_ROW:
            x_ship_position = FIRST_ENEMY_DEFAULT_X
            y_ship_position += Enemy.height
            id -= 1
            ships_count = 0

    return Game

#Function to navigate each node from the linked list and display the image of each ship on the screen

def show_ships(Game):

    Aux_List = LinkedList()
    Aux_List.head = Game.ships.head

    while Aux_List.head is not None:

        screen.blit(pygame.transform.scale(Aux_List.head.value.image, (Aux_List.head.value.width,
        Aux_List.head.value.height)), Aux_List.head.value.screen_pos)
        Aux_List.head = Aux_List.head.next

def move_row(Ships_List, time_function):

    Aux_List = LinkedList()
    Aux_List.head = Ships_List.head
    First_Position = Node()
    First_Position.next = Aux_List.head

    while Aux_List.head is not None:

        if type(Aux_List.head.value) is Enemy_Ship:

            Aux_List.head.value.move_enemy_ship(time_function)

        Aux_List.head = Aux_List.head.next

    Ships_List.head = First_Position.next

    return Ships_List

def right_border_check(Ships_List,max=0):

    Aux_List = LinkedList()
    Aux_List.head = Ships_List.head

    while Aux_List.head is not None:

        if type(Aux_List.head.value) is Enemy_Ship:

            if Aux_List.head.value.screen_pos[0] > max:

                max = Aux_List.head.value.screen_pos[0]

        Aux_List.head = Aux_List.head.next

    if max >= RIGHT_BORDER_LIMIT:

        return True

    else:

        return False

def left_border_check(Ships_List,min=RIGHT_BORDER_LIMIT):

    Aux_List = LinkedList()
    Aux_List.head = Ships_List.head

    while Aux_List.head is not None:

        if type(Aux_List.head.value) is Enemy_Ship:

            if Aux_List.head.value.screen_pos[0] < min:

                min = Aux_List.head.value.screen_pos[0]

        Aux_List.head = Aux_List.head.next

    if min >= 0:

        return False

    else:

        return True

def change_movement_direction(Ships_List):

    Aux_List = LinkedList()
    Aux_List.head = Ships_List.head
    First_Position = Node()
    First_Position.next = Aux_List.head

    while Aux_List.head is not None:

        if type(Aux_List.head.value) is Enemy_Ship:

            new_y = Aux_List.head.value.screen_pos[1] + Y_AXIS_MOVEMENT
            Aux_List.head.value.screen_pos = (Aux_List.head.value.screen_pos[0], new_y)
            Aux_List.head.value.speed *= -1.25

        Aux_List.head = Aux_List.head.next

    Ships_List.head = First_Position.next

    return Ships_List

def enemy_ship_explosion(screen_pos):

    frame = 1

    path = ".\images\Effects\Explosion\Explosion_"
    rect = pygame.Rect(screen_pos,(60,60))

    while frame <= 10:

        file_name = path + str(frame) + ".png"
        explosion_image = pygame.image.load(file_name)
        screen.blit(pygame.transform.scale(explosion_image, (60,60)), screen_pos)
        pygame.display.update(rect)

        frame += 1

    return False

def check_collision(Bullet,Game,explosion=False):

    Aux_List = LinkedList()
    Aux_List.head = Game.ships.head

    while Aux_List.head is not None:

        x_range = (Aux_List.head.value.screen_pos[0]+8, Aux_List.head.value.screen_pos[0]+40)
        y_range = (Aux_List.head.value.screen_pos[1], Aux_List.head.value.screen_pos[1] + Aux_List.head.value.height)

        if Bullet.screen_pos[0] >= x_range[0] and Bullet.screen_pos[0] <= x_range[1]:

            if Bullet.screen_pos[1] >= y_range[0] and Bullet.screen_pos[1] <= y_range[1]:

                explosion = (Aux_List.head.value.screen_pos)

                Game.score += Aux_List.head.value.id * 10
                Game.ships.delete(Aux_List.head.value)

                Bullet = None

                break

        Aux_List.head = Aux_List.head.next

    return Bullet, Game, explosion

pygame.font.init()
game_font = pygame.font.SysFont("arial", 18)

game_clock = pygame.time.Clock()

screen = pygame.display.set_mode(SCREEN_SIZE,0,32)
pygame.display.set_caption("Space Invaders")

second = 0
left_border = False
right_border = False
right_movement = True

Game_Space = game_start()

P_Bullet = None

explosion_position = False

while True:

    pressed_key = pygame.key.get_pressed()
    time_passed = game_clock.tick(60)
    time_seconds = time_passed / 1000.0
    second += time_seconds

    if second >= 1:

        Game_Space.ships = move_row(Game_Space.ships,0) #CHANGE ZERO TO TIME_SECONDS !!!!!!!!!!!!!!!!!!!!!!!

        if right_movement is True:

            right_border = right_border_check(Game_Space.ships)

        else:

            left_border = left_border_check(Game_Space.ships)

        second = 0

    if right_border is True:

        Game_Space.ships = change_movement_direction(Game_Space.ships)
        right_border = False
        right_movement = False

    if left_border is True:

        Game_Space.ships = change_movement_direction(Game_Space.ships)
        left_border = False
        right_movement = True

    for event in pygame.event.get():

        if event.type == QUIT:

            exit()

    screen.blit(Game_Space.background, (0,0))

    if P_Bullet is not None:

        P_Bullet.move_bullet(time_seconds)

        if P_Bullet.screen_pos[1] >= -20:

            P_Bullet, Game_Space, explosion_position = check_collision(P_Bullet, Game_Space)

            if P_Bullet is not None:

                screen.blit(P_Bullet.image, P_Bullet.screen_pos)

        else:

            P_Bullet = None

    if explosion_position is not False:

        explosion_position = enemy_ship_explosion(explosion_position)

    show_ships(Game_Space)

    Game_Space.show_text(game_font)

    for key_constant, is_pressed in enumerate(pressed_key):

        if is_pressed:

            if key_constant == 79: #right

                Game_Space.ships.head.value.move_player_ship(time_seconds)

            elif key_constant == 80: #left

                Game_Space.ships.head.value.move_player_ship(-time_seconds)

            elif key_constant == 44 and P_Bullet is None: #shoot

                P_Bullet = Game_Space.ships.head.value.player_shoot()

    pygame.display.update()


