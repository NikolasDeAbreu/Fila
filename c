import pygame
from pygame.locals import *
from sys import exit
import random
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
MAX_NUMBER_SHOOTS = 1
BULLET_UPPER_LIMIT = -20

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

    def __init__(self,player_ship=None,player_bullet=None,enemy_ships=LinkedList(),enemy_bullets=LinkedList(),
    bullets_count=0,score=0,score_position=(30,10),lives=3, lives_position=(550,10),stage=1,left_border=False,
    right_border=False,right_movement=True,game_clock=pygame.time.Clock()):

        self.player_ship = player_ship
        self.player_bullet = player_bullet
        self.enemy_ships = enemy_ships
        self.enemy_bullets = enemy_bullets
        self.bullets_count = bullets_count
        self.score = score
        self.score_position = score_position
        self.lives = lives
        self.lives_position = lives_position
        self.stage = stage
        self.left_border = left_border
        self.right_border = right_border
        self.right_movement = right_movement
        self.game_clock = game_clock

        background_file_name = r'.\images\background\background.png'
        self.background = pygame.image.load(background_file_name).convert()

    def game_start(self):

        self.player_ship = Player_Ship(1, PLAYER_DEFAULT_POSITION)
        self.player_ship.create_image("\g_obj1")
        id = 6  # Player ship has id 1. Enemy ships range from 2 to 6
        ships_count = 0
        x_ship_position, y_ship_position = FIRST_ENEMY_DEFAULT_POSITION

        while id > 1:  # It's done backwards because the stronger ships are the ones at the top of the screen

            ship_position = (x_ship_position, y_ship_position)
            Enemy = Enemy_Ship(id, ship_position)
            Enemy.create_image("\g_obj" + str(id))
            self.enemy_ships.add(Enemy)
            ships_count += 1
            x_ship_position += Enemy.width

            if ships_count == NUMBER_SHIPS_ROW:
                x_ship_position = FIRST_ENEMY_DEFAULT_X
                y_ship_position += Enemy.height
                id -= 1
                ships_count = 0

    def show_ships(self):

        Aux_List = LinkedList()
        Aux_List.head = self.enemy_ships.head
        delete_ship = None

        screen.blit(pygame.transform.scale(self.player_ship.image, (self.player_ship.width,self.player_ship.height)),
        self.player_ship.screen_pos)

        while Aux_List.head is not None:

            if Aux_List.head.value.destroyed is True:

                Aux_List.head.value.explosion_frame_update()

                if Aux_List.head.value.frame == 10:

                    delete_ship = Aux_List.head.value

            else:

                if self.bullets_count < MAX_NUMBER_SHOOTS:

                    shoot = random.random()

                    if shoot >= 0.98 and shoot <= 1:

                        Bullet = Aux_List.head.value.enemy_ship_shoot()

                        self.enemy_bullets.add(Bullet)

                        self.bullets_count += 1

            screen.blit(pygame.transform.scale(Aux_List.head.value.image, (Aux_List.head.value.width,
            Aux_List.head.value.height)),Aux_List.head.value.screen_pos)

            Aux_List.head = Aux_List.head.next

        if delete_ship is not None:

            self.enemy_ships.delete(delete_ship)

    def show_bullets(self):

        if self.enemy_bullets is not None:

            First_Position = Node()
            First_Position.next = self.enemy_bullets.head

            while self.enemy_bullets.head is not None:

                screen.blit(self.enemy_bullets.head.value.image,self.enemy_bullets.head.value.screen_pos)

                self.enemy_bullets.head = self.enemy_bullets.head.next

            self.enemy_bullets.head = First_Position.next

    def delete_out_of_screen(self):

        Aux_List = LinkedList()

        while self.enemy_bullets.head is not None:

            if self.enemy_bullets.head.value.out_of_screen is False:

                Aux_List.add(self.enemy_bullets.head.value)

            else:

                self.bullets_count -= 1

            self.enemy_bullets.head = self.enemy_bullets.head.next

        self.enemy_bullets.head = Aux_List.head

    def show_text(self,font):

        score_surface = font.render("Score: %s" % self.score, True, (255, 255, 255))
        screen.blit(score_surface, self.score_position)

        lives_surface = font.render("Lives: %s" % self.lives, True, (255, 255, 255))
        screen.blit(lives_surface, self.lives_position)

    def move_row(self, time_elapsed):

        First_Position = Node()
        First_Position.next = self.enemy_ships.head

        while self.enemy_ships.head is not None:

            self.enemy_ships.head.value.move_enemy_ship(time_elapsed)

            self.enemy_ships.head = self.enemy_ships.head.next

        self.enemy_ships.head = First_Position.next

    def move_bullets(self, time_elapsed):

        if self.enemy_bullets.head is not None:

            self.delete_out_of_screen()

            First_Position = Node()
            First_Position.next = self.enemy_bullets.head

            while self.enemy_bullets.head is not None:

                self.enemy_bullets.head.value.move_bullet(time_elapsed)

                if self.enemy_bullets.head.value.screen_pos[1] >= SCREEN_HEIGHT:

                    self.enemy_bullets.head.value.out_of_screen = True

                self.enemy_bullets.head = self.enemy_bullets.head.next

            self.enemy_bullets.head = First_Position.next
            self.check_collision()

        if self.player_bullet is not None:

            self.player_bullet.move_bullet(time_elapsed)
            self.check_upper_limit()

    def check_upper_limit(self):

        if self.player_bullet.screen_pos[1] >= BULLET_UPPER_LIMIT:

            self.check_collision()

        else:

            self.player_bullet = None

    def check_collision(self):

        if self.player_bullet is not None:

            Aux_List = LinkedList()
            Aux_List.head = self.enemy_ships.head

            Aux_Node = Node()
            Aux_Node.next = Aux_List.head

            while Aux_List.head is not None:

                x_range = (Aux_List.head.value.screen_pos[0] + 8, Aux_List.head.value.screen_pos[0] + 40)
                y_range = (Aux_List.head.value.screen_pos[1], Aux_List.head.value.screen_pos[1] + Aux_List.head.value.height)

                if self.player_bullet.screen_pos[0] >= x_range[0] and self.player_bullet.screen_pos[0] <= x_range[1]:

                    if self.player_bullet.screen_pos[1] >= y_range[0] and self.player_bullet.screen_pos[1] <= y_range[1]:

                        Aux_List.head.value.destroyed = True

                        self.score += Aux_List.head.value.id * 10

                        self.player_bullet = None

                        break

                Aux_List.head = Aux_List.head.next

            Aux_List.head = Aux_Node.next
            self.enemy_ships.head = Aux_List.head

            if self.player_bullet is not None:

                screen.blit(self.player_bullet.image,self.player_bullet.screen_pos)

        if self.enemy_bullets is not None:

            Aux_List = LinkedList()
            Aux_List.head = self.enemy_bullets.head

            Aux_Node = Node()
            Aux_Node.next = Aux_List.head

            while Aux_List.head is not None:

                ship_y_range = range(round(self.player_ship.screen_pos[1]),round(self.player_ship.screen_pos[1]+
                                                                                self.player_ship.height))
                ship_x_range = range(round(self.player_ship.screen_pos[0]+20),round(self.player_ship.screen_pos[0]+55))

                if round(Aux_List.head.value.screen_pos[0]) in ship_x_range:

                    if round(Aux_List.head.value.screen_pos[1]) in ship_y_range:

                        self.player_ship.destroyed = True
                        break

                Aux_List.head = Aux_List.head.next

            if self.player_ship.destroyed is True:

                self.restart_with_continue()

    def right_border_check(self, max=0):

        First_Position = Node()
        First_Position.next = self.enemy_ships.head

        while self.enemy_ships.head is not None:

            if self.enemy_ships.head.value.screen_pos[0] > max:

                max = self.enemy_ships.head.value.screen_pos[0]

            self.enemy_ships.head = self.enemy_ships.head.next

        self.enemy_ships.head = First_Position.next

        if max >= RIGHT_BORDER_LIMIT:

            self.right_border = True

    def left_border_check(self, min=RIGHT_BORDER_LIMIT):

        First_Position = Node()
        First_Position.next = self.enemy_ships.head

        while self.enemy_ships.head is not None:

            if self.enemy_ships.head.value.screen_pos[0] < min:

                min = self.enemy_ships.head.value.screen_pos[0]

            self.enemy_ships.head = self.enemy_ships.head.next

        self.enemy_ships.head = First_Position.next

        if min < 0:

            self.left_border = True

    def change_movement_direction(self):

        First_Position = Node()
        First_Position.next = self.enemy_ships.head

        while self.enemy_ships.head is not None:

            new_y = self.enemy_ships.head.value.screen_pos[1] + Y_AXIS_MOVEMENT
            self.enemy_ships.head.value.screen_pos = (self.enemy_ships.head.value.screen_pos[0], new_y)

            self.enemy_ships.head.value.speed *= -1.25
            self.enemy_ships.head.value.bullet_speed += 5

            self.enemy_ships.head = self.enemy_ships.head.next

        self.enemy_ships.head = First_Position.next

        if self.right_movement is True:

            self.right_border = False
            self.right_movement = False

        else:

            self.left_border = False
            self.right_movement = True

    def restart_with_continue(self):

        self.player_ship.death_animation()

        self.player_ship.screen_pos = PLAYER_DEFAULT_POSITION
        self.player_ship.create_image("\g_obj1")
        self.player_ship.destroyed = False
        self.player_ship.frame = 1
        self.lives -= 1

        self.bullets_count = 0
        self.enemy_bullets.head = None

        self.game_clock.tick(60)


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

    def __init__(self,id,screen_pos,image=None,width=100,height=100,speed=100,destroyed=False,frame=1):
        super().__init__(id,screen_pos,image)
        self.width = width
        self.height = height
        self.speed = speed
        self.destroyed = destroyed
        self.frame = 1

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

    def death_animation(self):

        image_path = ".\images\Effects\Explosion\Explosion_"

        while self.frame <= 10:

            image_name = image_path + str(self.frame) + ".png"
            self.image = pygame.image.load(image_name)
            screen.blit(pygame.transform.scale(self.image,(self.width,self.height)),self.screen_pos)
            pygame.display.update(self.screen_pos,(self.width,self.height))
            pygame.time.delay(75)

            self.frame += 1

class Player_Bullet(GameObject):

    def __init__(self, id, screen_pos, image=None,speed=275):
        super().__init__(id,screen_pos,image)
        self.speed = speed

    def __repr__(self):

        return "(%s,%s) / %s" % (self.screen_pos[0],self.screen_pos[1],self.image)

    def move_bullet(self,time_elapsed):

        distance = self.speed * time_elapsed
        new_y = self.screen_pos[1] - distance
        self.screen_pos = (self.screen_pos[0],new_y)

class Enemy_Ship(GameObject):

    def __init__(self,id,screen_pos,image=None,width=60,height=60,speed=225,destroyed=False,frame=1,bullet_speed=40):
        super().__init__(id,screen_pos,image)
        self.width = width
        self.height = height
        self.speed = speed
        self.destroyed = destroyed
        self.frame = frame
        self.bullet_speed = bullet_speed

    def move_enemy_ship(self,time_elapsed):

        distance = self.speed * time_elapsed
        new_x = self.screen_pos[0] + distance

        if (new_x + self.width) <= SCREEN_WIDTH:

            self.screen_pos = (new_x, self.screen_pos[1])

    def explosion_frame_update(self):

        image_path = ".\images\Effects\Explosion\Explosion_"
        image_name = image_path + str(self.frame) + ".png"
        self.image = pygame.image.load(image_name)

        if self.frame < 10:

            self.frame += 1

    def enemy_ship_shoot(self):

        if self.id in (5, 6):

            bullet_x = self.screen_pos[0] + 10

        elif self.id in (3, 4):

            bullet_x = self.screen_pos[0] + 18

        elif self.id == 2:

            bullet_x = self.screen_pos[0] + 20

        bullet_speed = self.id * 20 + self.bullet_speed

        Bullet = Enemy_Bullet(self.id, (bullet_x, self.screen_pos[1]+60),None,bullet_speed)
        Bullet.create_image(r'\bullet'+str(self.id))

        return Bullet

class Enemy_Bullet(GameObject):

    def __init__(self, id, screen_pos, image=None,speed=100,out_of_screen=False):
        super().__init__(id,screen_pos,image)
        self.speed = speed
        self.out_of_screen = out_of_screen

    def __repr__(self):

        return "(%s,%s) / %s" % (self.screen_pos[0],self.screen_pos[1],self.image)

    def move_bullet(self,time_elapsed):

        distance = self.speed * time_elapsed
        new_y = self.screen_pos[1] + distance
        self.screen_pos = (self.screen_pos[0],new_y)

pygame.font.init()
game_font = pygame.font.SysFont("arial", 18)

screen = pygame.display.set_mode(SCREEN_SIZE,0,32)
pygame.display.set_caption("Space Invaders")

second = 0

Game_Space = Space()
Game_Space.game_start()

while True:

    if Game_Space.enemy_ships.head is None:

        Game_Space.next_level()

    pressed_key = pygame.key.get_pressed()
    time_passed = Game_Space.game_clock.tick(60)
    time_seconds = time_passed / 1000.0
    second += time_seconds

    if second >= 1:

        Game_Space.move_row(0) #CHANGE ZERO TO TIME_SECONDS !!!!!!!!!!!!!!!!!!!!!!!

        if Game_Space.right_movement is True:

            Game_Space.right_border_check()

        else:

            Game_Space.left_border_check()

        second = 0

    if Game_Space.right_border is True:

        Game_Space.change_movement_direction()

    if Game_Space.left_border is True:

        Game_Space.change_movement_direction()

    for event in pygame.event.get():

        if event.type == QUIT:

            exit()

    screen.blit(Game_Space.background, (0,0))

    Game_Space.move_bullets(time_seconds)
    Game_Space.show_ships()
    Game_Space.show_bullets()
    Game_Space.show_text(game_font)

    for key_constant, is_pressed in enumerate(pressed_key):

        if is_pressed:

            if key_constant == 79: #right

                Game_Space.player_ship.move_player_ship(time_seconds)

            elif key_constant == 80: #left

                Game_Space.player_ship.move_player_ship(-time_seconds)

            elif key_constant == 44 and Game_Space.player_bullet is None: #shoot

                Game_Space.player_bullet = Game_Space.player_ship.player_shoot()

    pygame.display.update()


