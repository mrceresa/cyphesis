#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from whrandom import *
from mind.panlingua import interlinguish
il=interlinguish
from world import probability
from editor import editor
from Quaternion import Quaternion
from Vector3D import Vector3D
import time

#goal priority
#1) eating: certain times
#2) market/tavern: certain times, certain probability
#3) sleeping: nights
#4) chop trees: winter (when not doing anything else)
#4) other similar tasks: seasonal (-"-)

#'Breakfast' goal is type of 'eating'.

# Heights are all 0 for now, as uclient doesn't differentiate
# Once clients and servers can handle terrain properlly, then we
# can start thinking in more ernest about heights
settlement_height=0
forest_height=0

hall_xyz=(5,3,settlement_height)
forest_xyz=(-20,-60,settlement_height)

pig_sty_xyz=(8,8,settlement_height)
butcher_stall_xyz=(-41.5,-6.3,settlement_height)

knowledge=[('axe','place','smithy'),
           ('forest','location',forest_xyz),
           ('hall','location',hall_xyz)]
mprices=[('pig','price','5')]
bprices=[('ham','price','2')]
bknowledge=[('market','location',butcher_stall_xyz)]
mknowledge=[('market','location',pig_sty_xyz)]
sknowledge=[('forest','location',forest_xyz),
            ('stash','location',(-98,-97,settlement_height))]
village=[('hall','location', hall_xyz),
         ('butcher','location', butcher_stall_xyz),
         ('pig','location', pig_sty_xyz)]
gknowledge=[('m1','location',(-17, -1,    settlement_height)),
            ('m2','location',(-29, -1,    settlement_height)),
            ('m3','location',(-29, -7.5,  settlement_height)),
            ('m4','location',(-38, -10,   settlement_height)),
            ('m5','location',(-43, -15,   settlement_height)),
            ('m6','location',(-43, -14.5, settlement_height))]

wolf_knowledge=[('w1','location',(90,-90,settlement_height)),
                ('w2','location',(110,-90,settlement_height)),
                ('w3','location',(110,90,settlement_height)),
                ('w4','location',(90,90,settlement_height))]

lych_knowledge=[('w1','location',(0,-96,settlement_height)),
                ('w2','location',(-70,-70,settlement_height)),
                ('w3','location',(-100,70,settlement_height)),
                ('w4','location',(-147,-90,settlement_height))]

wander=(il.wander,"wander()")
forage=(il.forage,"forage()")
trade=(il.trade,"trade()")
keep=(il.keep,"keep()")
sell=(il.sell,"sell_trade()")
patrol=(il.patrol,"patrol()")

pig_goals=[(il.avoid,"avoid(['wolf','skeleton','crab'],10.0)"),
           (il.forage,"forage(self, 'acorn')"),
           (il.forage,"forage(self, 'apple')"),
           (il.forage,"forage(self, 'mushroom')"),
           (il.herd,"herd()")]

deer_goals=[(il.avoid,"avoid(['settler','orc'],10.0)"),
            (il.forage,"forage(self, 'apple')"),
            (il.forage,"forage(self, 'mushroom')"),
            (il.browse,"browse(self, 'fir', 0.8)"),
            (il.flock,"flock()")]

chicken_goals=[(il.avoid,"avoid(['settler','orc','wolf'],10.0)"),
               (il.flock,"flock()")]

squirrel_goals=[(il.avoid,"avoid(['wolf','crab'],10.0)"),
                (il.forage,"forage(self, 'acorn')"),
                (il.forage,"forage(self, 'pinekernel')")]

wolf_goals=[(il.forage,"forage(self, 'ham')"),
            (il.hunt,"predate(self,'pig',30.0)"),
            (il.hunt,"predate(self,'crab',20.0)"),
            (il.hunt,"predate(self,'squirrel',10.0)"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]

crab_goals=[(il.avoid,"avoid('wolf',10.0)"),
            (il.hunt,"predate_small(self,'pig',30.0,10.0)")]

lych_goals=[(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]


# N, E, S, W, NE, SE, SW, NW in order
directions = [[0,0,0.707,0.707],[0,0,0,1],[0,0,-0.707,0.707],[0,0,1,0],
              [0,0,0.387,0.921],[0,0,-0.387,0.921],[0,0,-0.921,0.387],[0,0,0.921,0.387]]

forests = [
           ('oak', 20, 20, 200, -100, 100, 20),
           ('oak', 20, -100, 100, 20, 200, 20),
           ('fir', 200,  200,  300, -300, 300, 50),
           ('fir', 200, -300, 300,  200,  300, 50)
          ]

#observer calls this
def default(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()

    points = { }
    for i in range(-5, 6):
        for j in range(-5, 6):
            if i==5 or j==5:
                points['%ix%i'%(i,j)] = [i, j, uniform(100, 150)]
            elif i==-5 or j == -5:
                points['%ix%i'%(i,j)] = [i, j, uniform(-30, -10)]
            elif (i==2 or i==3) and (j==2 or j==3):
                points['%ix%i'%(i,j)] = [i, j, uniform(20, 25)]
            elif i==4 or j==4:
                points['%ix%i'%(i,j)] = [i, j, uniform(30, 80)]
            elif i==-4 or j==-4:
                points['%ix%i'%(i,j)] = [i, j, uniform(-2, 5)]
            else:
                points['%ix%i'%(i,j)] = [i, j, 1+uniform(0, 8)*(abs(i)+abs(j))]

    points['-1x-1'] = [-1, -1, -16.8]
    points['0x-1'] = [0, -1, -3.8]
    points['-1x0'] = [-1, 0, -2.8]
    points['-1x1'] = [-1, 1, 12.8]
    points['1x-1'] = [1, -1, 15.8]
    points['0x0'] = [0, 0, 12.8]
    points['1x0'] = [1, 0, 23.1]
    points['0x1'] = [0, 1, 14.2]
    points['1x1'] = [1, 1, 19.7]

    m.set(world.id, terrain={'points' : points}, name="moraf")

# a wall around the world

    m.make('boundary',type='boundary',xyz=(-257,-257,settlement_height),bbox=[2,514,256])
    m.make('boundary',type='boundary',xyz=(-257,-257,settlement_height),bbox=[514,2,256])
    m.make('boundary',type='boundary',xyz=(-257, 256,settlement_height),bbox=[514,2,256])
    m.make('boundary',type='boundary',xyz=( 256,-257,settlement_height),bbox=[2,514,256])

    m.make('fir',type='fir',xyz=(-10,-0,settlement_height))
    m.make('fir',type='fir',xyz=(-0,-10,settlement_height))
    m.make('fir',type='fir',xyz=(0,10,settlement_height))
    m.make('fir',type='fir',xyz=(10,0,settlement_height))

    chickens=[]
    xbase = uniform(12,20)
    ybase = uniform(12,20)
    for i in range(0, 10):
        xpos = xbase + uniform(-5,5)
        ypos = ybase + uniform(-5,5)
        d=m.make('chicken', type='chicken', xyz=(xpos, ypos, settlement_height))
        chickens.append(d)
    m.learn(chickens,chicken_goals)
    
    # m.make('sherwood',type='forest',xyz=(-50, 10,settlement_height),bbox=[40,40,40])

    m.make('jetty',type='jetty',xyz=(-22,-48,0))
    m.make('boat',type='boat',xyz=(-22,-56,0),mode="floating")

#def dontrunme():
# a camp near the origin

    #cfire=m.make('campfire',type='campfire',xyz=(0,4,settlement_height))
    #m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)
    #m.make('tent',type='tent',xyz=(-1,8,settlement_height),bbox=[2.5,2.5,3])
    #m.make('lumber',type='lumber',xyz=(-1,3,settlement_height))
    #m.make('lumber',type='lumber',xyz=(-1,2.5,settlement_height))

    # hall=m.make('hall',type='hall',xyz=hall_xyz)

    # Fire in the centre of the hall
    # cfire=m.make('campfire',type='campfire',xyz=(6,6,settlement_height),
                                            # parent=hall.id)
    # m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    cfire=m.make('campfire',type='campfire',xyz=(3,9,settlement_height))
    m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    cfire=m.make('campfire',type='campfire',xyz=(11,1,settlement_height))
    m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    for i in range(0, 20):
        m.make('lumber',type='lumber',xyz=(uniform(-200,0),uniform(-200,0),settlement_height))

    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],type=i[0],xyz=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)])

    m.make('weather',type='weather',desc='object that describes the weather',
           xyz=(0,1,0), rain=0.0)

#   bones all over the place
    for i in range(0, 10):
        xpos = uniform(-200,200)
        ypos = uniform(-200,200)
        m.make('skull', type='skull', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('pelvis', type='pelvis', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('arm', type='arm', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('thigh', type='thigh', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('shin', type='shin', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('ribcage', type='ribcage', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))

#   the lych, who makes bones into skeletons
    lych=m.make('lych', type='lych', xyz=(-21, -89, settlement_height))
    m.learn(lych,lych_goals)
    m.know(lych,lych_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

#   animals
    piglet = m.make('pig', type='pig', xyz=(-3,-1,settlement_height))
    m.learn(piglet,pig_goals)

    wolf = m.make('wolf', type='wolf', xyz=(90,-90,settlement_height))
    m.learn(wolf,wolf_goals)
    m.know(wolf,wolf_knowledge)
    m.tell_importance(wolf,il.forage,'>',il.hunt)
    m.tell_importance(wolf,il.forage,'>',il.patrol)
    m.tell_importance(wolf,il.hunt,'>',il.patrol)

    crab = m.make('crab', type='crab', xyz=(-90,90,settlement_height))
    m.learn(crab,crab_goals)

    skeleton = m.make('skeleton', type='skeleton', xyz=(-38,-25,settlement_height))

    squirrel = m.make('squirrel', type='squirrel', desc='test squirrel',
                    xyz=(-32,-15,settlement_height))
    m.know(squirrel,sknowledge)
    m.learn(squirrel,squirrel_goals)

#   villagers
    #directions = [[0,1,0],[1,0,0],[0,-1,0],[-1,0,0],
                  #[0.7,0.7,0],[0.7,-0.7,0],[-0.7,-0.7,0],[-0.7,0.7,0]]

    home1_xyz=(90,-90,settlement_height)

    butcher=m.make('Ulad Bargan',type='butcher',desc='the butcher',
                 xyz=butcher_stall_xyz,age=probability.fertility_age,sex='male')
    m.learn(butcher,(il.trade,"trade(self, 'pig', 'cleaver', 'cut', 'ham', 'market','day')"))
    m.learn(butcher,(il.buy_livestock,"buy_livestock('pig', 1)"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','open','dawn')"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','closed','evening')"))
    m.know(butcher,bknowledge)
    m.know(butcher,bprices)
    cleaver=m.make('cleaver', type='cleaver', desc='cleaver for cutting meat',
                   place='market', xyz=(-41,-5,settlement_height))
    m.own(butcher,cleaver)
    m.learn(butcher,(il.sell,"sell_trade('ham', 'market')"))
    coins=[]
    for i in range(0, 60):
        coins.append(m.make('coin',type='coin',xyz=(0,0,0),parent=butcher.id))
    m.own(butcher,coins)
    

    home2_xyz=(80,80,settlement_height)
    merchant=m.make('Dyfed Searae',type='merchant',desc='the pig merchant',
                    xyz=pig_sty_xyz,age=probability.fertility_age,
                    sex='male',orientation=Quaternion(Vector3D([1,0,0]),Vector3D([-1,0,0])).as_list())
    sty=m.make('sty',type='sty',xyz=pig_sty_xyz,status=1.0,bbox=[5,5,3])
    m.know(merchant,mknowledge)
    m.know(merchant,village)
    m.know(merchant,mprices)
    m.own(merchant,sty)
    m.learn(merchant,(il.keep,"keep_livestock('pig', 'sty', 'sowee')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market', 'morning')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market', 'afternoon')"))
    m.learn(merchant,(il.lunch,"meal(self, 'ham','midday', 'inn')"))
    m.learn(merchant,(il.sup,"meal(self, 'beer', 'evening', 'inn')"))
    m.learn(merchant,(il.welcome,"welcome('Welcome to this our settlement','settler')"))
    m.learn(merchant,(il.help,"add_help(['Thankyou for joining our remote settlement.','Our first task is to build some shelter, but while we are doing that we still need food.','You can help us out by raising pigs for slaughter.','If you want to buy a piglet to raise, let me know by saying you would like to buy one.','Pigs love to eat acorns from under the oak trees that are abundant in this area.'])"))
    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',type='pig',xyz=(uniform(0,4),uniform(0,4),settlement_height),parent=sty.id,orientation=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

    # Warriors - the more adventurous types

    warriors=[]
    warrior=m.make('Vonaa Barile',type='mercenary',xyz=(uniform(-2,2),uniform(-2,2),settlement_height),sex='female',orientation=directions[randint(0,7)])
    bow=m.make('bow',type='bow',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    warriors.append(warrior)

    warrior=m.make('Lile Birloc', type='mercenary',xyz=(uniform(-2,2),uniform(-2,2),settlement_height),sex='female',orientation=directions[randint(0,7)])
    bow=m.make('bow',type='bow',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    for i in range(0, 6):
        arrow=m.make('arrow',type='arrow',xyz=(0,0,0), parent=warrior.id)
        m.own(warrior,arrow)
    warriors.append(warrior)

    # Warriors all know where stuff is in the village
    m.know(warriors,village)

    # Warriors enjoy their food and drink
    m.price(warriors, [('services','5')])
    m.learn(warriors,(il.help,"add_help(['The forest is a dangerous place.','If you need some help protecting your pigs,','I can help you out for a day or so.','I will need some gold for food and equipment.','For 5 coins I can work for you until sundown.','After sundown you should make sure your pigs are safe,','and get indoors yourself.','If you want to hire my services,','let me know by saying you would like to hire me.'])"))
    m.learn(warriors,(il.hire,"hire_trade()"))
    m.learn(warriors,(il.lunch,"meal(self, 'ham','midday', 'inn')"))
    m.learn(warriors,(il.sup,"meal(self, 'beer', 'evening', 'inn')"))

    deers=[]
    xbase = uniform(-180,180)
    ybase = uniform(-180,180)
    for i in range(0, 10):
        xpos = xbase + uniform(-20,20)
        ypos = ybase + uniform(-20,20)
        d=m.make('deer', type='deer', xyz=(xpos, ypos, settlement_height))
        deers.append(d)
    m.learn(deers,deer_goals)
    

    # I am not sure if we need a guard
    #m.learn(guard,(il.patrol,"patrol(['m1', 'm2', 'm3', 'm4', 'm5', 'm6'])"))
    #m.tell_importance(guard,il.defend,'>',il.patrol)

def add_pigs(mapeditor):
#   general things

    m=editor(mapeditor)

    sty = m.look_for(type='sty')
    merchant = m.look_for(name='Dyfed Searae')

    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',type='pig',xyz=(uniform(0,4),uniform(0,4),settlement_height),parent=sty.id,orientation=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

def add_memtest(mapeditor):
#   general things

    m=editor(mapeditor)

    m.make('settler',type='settler',xyz=(0,5,5))
    m.make('oak',type='oak',xyz=(5,0,5))

def add_village(mapeditor):
#   general things

    m=editor(mapeditor)

    m.make('tower',type='tower',xyz=(210,210,5))
    m.make('gallows',type='gallows',xyz=(185,175,5))

    m.make('house3',type='house3',xyz=(158,150,22),orientation=directions[1])
    m.make('house3',type='house3',xyz=(158,158,22),orientation=directions[4])
    m.make('house3',type='house3',xyz=(150,158,22),orientation=directions[0])
    m.make('house3',type='house3',xyz=(142,158,22),orientation=directions[7])
    m.make('house3',type='house3',xyz=(142,150,22),orientation=directions[3])
    m.make('house3',type='house3',xyz=(142,142,22),orientation=directions[6])
    m.make('house3',type='house3',xyz=(150,142,22),orientation=directions[2])
    m.make('house3',type='house3',xyz=(158,142,22),orientation=directions[5])

def test_pig(mapeditor):
#   general things

    m=editor(mapeditor)
    pig = m.make('pig', type='pig', xyz=(3,3,settlement_height))
    m.learn(pig,pig_goals)
    m.make('acorn', type='acorn', xyz=(4,4,settlement_height))

def test_browse(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    deer = m.make('deer', type='deer', xyz=(5, 0, settlement_height))
    m.learn(deer, (il.browse,"browse(self, 'fir', 0.8)"))
    m.make('fir',type='fir',xyz=(-10,-0,settlement_height))
    m.make('fir',type='fir',xyz=(-0,-10,settlement_height))
    m.make('fir',type='fir',xyz=(0,10,settlement_height))
    m.make('fir',type='fir',xyz=(10,0,settlement_height))
    
def test_forest(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],type=i[0],xyz=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)])

def modify_terrain(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()
    points = { }
    for i in range(-5, 6):
        for j in range(-5, 6):
            if i==5 or j==5:
                points['%ix%i'%(i,j)] = [i, j, uniform(100, 150)]
            elif i==-5 or j == -5:
                points['%ix%i'%(i,j)] = [i, j, uniform(-30, -10)]
            elif (i==2 or i==3) and (j==2 or j==3):
                points['%ix%i'%(i,j)] = [i, j, uniform(20, 25)]
            elif i==4 or j==4:
                points['%ix%i'%(i,j)] = [i, j, uniform(30, 80)]
            elif i==-4 or j==-4:
                points['%ix%i'%(i,j)] = [i, j, uniform(-2, 5)]
            else:
                points['%ix%i'%(i,j)] = [i, j, 1+uniform(0, 8)*(abs(i)+abs(j))]

    points['-1x-1'] = [-1, -1, -16.8]
    points['0x-1'] = [0, -1, -3.8]
    points['-1x0'] = [-1, 0, -2.8]
    points['-1x1'] = [-1, 1, 12.8]
    points['1x-1'] = [1, -1, 15.8]
    points['0x0'] = [0, 0, 12.8]
    points['1x0'] = [1, 0, 23.1]
    points['0x1'] = [0, 1, 14.2]
    points['1x1'] = [1, 1, 19.7]

    m.set(world.id, terrain={'points' : points})

def test_coll(mapeditor):

    m=editor(mapeditor)

    sty=m.make('sty',type='sty',xyz=pig_sty_xyz,status=1.0,bbox=[5,5,3], orientation=directions[0])

def test_deer(mapeditor):

    m=editor(mapeditor)

    deer=m.make('deer', type='deer', xyz=(10, 10, settlement_height))
    m.learn(deer,deer_goals)

    chicken=m.make('chicken', type='chicken', xyz=(5, 10, settlement_height))
    m.learn(chicken,chicken_goals)
