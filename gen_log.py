import json
import time
import copy
import numpy as np
import random


def save_log_as_json(file_name,
                 electric_type='0100',
                 long_addr='',
                 addr='ABCD',
                 opt='send',
                 state=0,
                 sub_seq=1,
                 mode_index=0,
                 data='',
                 time=0):
    # create a dictionary
    log = {}
    log['type'] = electric_type
    log['long_addr'] = long_addr
    log['addr'] = addr
    log['opt'] = opt
    info = {}
    info['act'] = None
    info['state'] = state
    info['sub_seq'] = sub_seq
    info['mode_index'] = mode_index
    info['data'] = data
    log['info'] = info
    log['time'] = time

    # transfer the python object to json
    log_json = json.dumps(log)

    # open a file to save the log
    f = open(file_name, 'a')
    f.write(log_json)
    f.write('\n')
    f.close()


def save_logs_as_json(file_name, orders):
    f = open('log/' + file_name, 'w')
    for order in orders:
        log_json = json.dumps(order)
        f.write(log_json)
        f.write('\n')
    f.close()


def save_as_json(file_name, addr):
    with open(file_name, 'w') as f:
        json.dump(addr, f)

    # transfer to json(old style, my python need to be improved...)
    '''addr_json = json.dumps(addr)

    # open a file to save the address
    f = open(file_name, 'w')
    f.write(addr_json)
    f.close()'''


def gaussian_distribution(mean, square, size):
    std_norm = np.random.randn(size)
    shifted_norm = std_norm*square + mean
    return shifted_norm.astype('int32')


def gen_log_from_excel():
    # read the scenes
    data = open_excel('sceneelectrics.xlsx')
    table = data.sheet_by_name(u'Sheet1')
    print 'scene electrics total lines:', table.nrows

    se_electric_code = table.col_values(4)[1:]
    se_electric_order = table.col_values(5)[1:]
    se_order_info = table.col_values(6)[1:]
    se_scene_index = [int(x) for x in table.col_values(9)[1:]]

    # dictionary for saving the address
    addr = {}
    for i in range(0, table.nrows - 1):
        long_addr = str(se_electric_code[i][4:])
        addr[long_addr] = 'ABCD'

    # save the scene mode
    mode = {}
    for i in range(0, table.nrows - 1):
        if mode.get(se_scene_index[i], -1) == -1:
            mode[se_scene_index[i]] = [(se_electric_code[i], se_electric_order[i], se_order_info[i])]
        else:
            mode[se_scene_index[i]].append((se_electric_code[i], se_electric_order[i], se_order_info[i]))

    # read the electric orders
    data = open_excel('electricorders.xlsx')
    table = data.sheet_by_name(u'Sheet1')
    print 'electric orders total lines:', table.nrows

    eo_electric_code = table.col_values(2)[1:]
    eo_order_data = table.col_values(3)[1:]
    eo_order_info = table.col_values(4)[1:]
    eo_time = table.col_values(5)[1:]

    for i in range(0, table.nrows - 1):
        long_addr = str(eo_electric_code[i][4:])
        addr[long_addr] = 'ABCD'

    # save the address as json file
    save_as_json('addr.json', addr)

    for i in range(0, table.nrows - 1):
        # print '----------------------------------------'
        electric_type = eo_electric_code[i][:4]
        long_addr = eo_electric_code[i][4:]
        order = eo_order_data[i]
        data = eo_order_info[i]
        write_time = eo_time[i][:10]

        time_array = time.strptime(eo_time[i], '%Y-%m-%d %H:%M:%S')
        time_stamp = int(time.mktime(time_array))

        file_name = 'log/' + write_time + '.log'
        if order == 'TH':
            mode_index = int(data[2])
            i = 0
            scene_mode = mode.get(mode_index, -1)
            if scene_mode == -1:
                continue
            for (m_electric_code, m_electric_order, m_electric_info) in scene_mode:
                electric_type = m_electric_code[:4]
                long_addr = m_electric_code[4:]
                data = m_electric_info
                sub_seq = int(data[1])
                state = 1 << (sub_seq - 1)
                save_log_as_json(file_name, electric_type=electric_type,
                                 long_addr=long_addr, state=state,
                                 sub_seq=sub_seq, mode_index=mode_index,
                                 data=data, time=time_stamp+i)
                i += 1
        else:
            if order == 'XH' and (electric_type == '0100' or electric_type == '0200' or electric_type == '0300'):
                sub_seq = max(int(data[1]), 1)
                state = 1 << (sub_seq - 1)
            elif order == 'XG' and (electric_type == '0100' or electric_type == '0200' or electric_type == '0300'):
                sub_seq = int(data[1])
                state = 0
            else:
                sub_seq = 1
                state = 0
            save_log_as_json(file_name, electric_type=electric_type,
                             long_addr=long_addr, state=state,
                             sub_seq=sub_seq, data=data, time=time_stamp)


def gen_log_randomly(file_names):
    num_dev = 100
    num_mode = 10

    # generate address randomly
    addr = {}
    addr_list = []
    for i in range(num_dev):
        long_addr = random.randint(0, 0xFFFFFFFF)
        short_addr = random.randint(0, 0xFFFF)
        long_addr_str = '%08X' % long_addr
        short_addr_str = '%04X' % short_addr
        addr[long_addr_str] = short_addr_str
        addr_list.append((long_addr_str, short_addr_str))

    print addr
    save_as_json('config/addr.json', addr)

    # generate mode randomly
    mode_index = {}
    info = {'act': 'mod',
            'state': 0,
            'sub_seq': 1}
    order = {'type': '0100',
             'opt': 'send',
             'info': info}
    mode = {'enable': 1}
    mode_len = 8
    mode_assembly = np.random.choice(num_dev, mode_len * num_mode, replace=False)
    start = 0
    for i in range(num_mode):
        mode_index[i] = mode_assembly[start:start+mode_len]
        orders = []
        for addr_index in mode_index[i]:
            order['long_addr'] = addr_list[addr_index][0]
            order['addr'] = addr_list[addr_index][1]
            order['info']['mode_index'] = i
            order['info']['data'] = '01%d******FF' % i
            orders.append(copy.deepcopy(order))
        mode['orders'] = orders
        print mode
        save_as_json('config/mode%d.json' % i, mode)
        start += mode_len

    # generate log randomly
    num_habit = 4
    total_time = 24*3600
    time_unit = 100
    m = total_time / time_unit

    mode_param = {'act_times': 3, 'time_last': (1800, 600), 'time_varies': (0, 100)}
    habit_param = {'act_prob': 0.7, 'time_last': (1800, 600), 'time_varies': (0, 100)}
    noise_param = {'act_times': 3, 'time_last': (60, 100)}

    habit_mode_exe_time = np.random.choice(total_time, num_habit, replace=False)
    for f in file_names:
        time_array = time.strptime(f[:-4] + ' 00:00:00', '%Y-%m-%d %H:%M:%S')
        date_time = int(time.mktime(time_array))
        log_orders = []
        # habit mode
        prob_exe = habit_param['act_prob']
        for h in range(num_habit):
            if np.random.choice([0, 1], p=[1-prob_exe, prob_exe]) == 1:
                exe_time_varies = gaussian_distribution(habit_param['time_varies'][0],
                                                        habit_param['time_varies'][1],
                                                        mode_len)
                last_time = gaussian_distribution(habit_param['time_last'][0],
                                                  habit_param['time_last'][1],
                                                  mode_len)
                for i, o in enumerate(mode_index[h]):
                    order['long_addr'] = addr_list[o][0]
                    order['addr'] = addr_list[o][1]
                    order['info']['mode_index'] = h
                    order['info']['state'] = 1
                    order['info']['data'] = '01%d******FF' % h
                    order['time'] = date_time + habit_mode_exe_time[h] + exe_time_varies[i]
                    log_orders.append(copy.deepcopy(order))
                    order['info']['state'] = 0
                    order['time'] += last_time[i]
                    log_orders.append(copy.deepcopy(order))

        # normal mode
        prob_exe = mode_param['act_times'] * 1.0 / m
        '''for h in range(num_habbit, num_mode):
            for t in range(m):
                if np.random.choice([0, 1], p=[1 - prob_exe, prob_exe]) == 1:'''
        for r in range(mode_param['act_times']):
            act_times = np.random.choice(m, num_mode - num_habit)
            for h, t in enumerate(act_times):
                if True:
                    exe_time_varies = gaussian_distribution(mode_param['time_varies'][0],
                                                            mode_param['time_varies'][1],
                                                            mode_len)
                    last_time = gaussian_distribution(mode_param['time_last'][0],
                                                      mode_param['time_last'][1],
                                                      mode_len)
                    for i, o in enumerate(mode_index[h + num_habit]):
                        order['long_addr'] = addr_list[o][0]
                        order['addr'] = addr_list[o][1]
                        order['info']['mode_index'] = h + num_habit
                        order['info']['state'] = 1
                        order['info']['data'] = '01%d******FF' % (h + num_habit)
                        order['time'] = date_time + t * time_unit + exe_time_varies[i]
                        log_orders.append(copy.deepcopy(order))
                        order['info']['state'] = 0
                        order['time'] += last_time[i]
                        log_orders.append(copy.deepcopy(order))

        # noise
        prob_exe = noise_param['act_times'] * 1.0 / m
        '''for t in range(m):
            for o in range(num_dev):
                if np.random.choice([0, 1], p=[1 - prob_exe, prob_exe]) == 1:'''
        for r in range(noise_param['act_times']):
            act_times = np.random.choice(m, num_dev)
            for o, t in enumerate(act_times):
                if True:
                    last_time = gaussian_distribution(noise_param['time_last'][0],
                                                      noise_param['time_last'][1],
                                                      num_dev)
                    order['long_addr'] = addr_list[o][0]
                    order['addr'] = addr_list[o][1]
                    order['info']['mode_index'] = 0
                    order['info']['state'] = 1
                    order['info']['data'] = '010******FF'
                    order['time'] = t * time_unit + date_time
                    log_orders.append(copy.deepcopy(order))
                    order['info']['state'] = 0
                    order['time'] += last_time[o]
                    log_orders.append(copy.deepcopy(order))

        print '%s generate finished' % f

        # sort the orders by time and save as json file
        log_orders.sort(key=lambda x: x['time'])
        save_logs_as_json(f, log_orders)


if __name__ == '__main__':
    file_names = []
    for month in range(7, 13):
        for day in range(1, 32):
            if (month == 9 or month == 11) and day == 31:
                continue
            f = '2018-%02d-%02d.log' % (month, day)
            file_names.append(f)
    gen_log_randomly(file_names)
