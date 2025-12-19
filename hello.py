from flask import Flask, render_template, request, jsonify, abort
import json
app = Flask(__name__)

import serial
import time

try:
    ser = serial.Serial(
        port = '/dev/serial0',
        baudrate = 115200,
        parity = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS,
        timeout = 2
    )
    print("Port série ouvert avec succès")
except Exception as e:
    print(f"Erreur ouverture port série : {e}")
    ser = None  # En cas d'erreur, on désactive la série

@app.route('/')
def hello_world():
    return 'Hello, World!\n'

welcome = "Welcome to 3ESE API!"
temperatures = []
pressions = []
K = 1.0         #échelle par défault, multiplicateur pour l'angle


@app.route('/api/welcome/')
def api_welcome():
    return welcome

@app.route('/api/welcome/<int:index>', methods=['GET','POST'])
def api_welcome_index(index):
    if index < 0 or index >= len(welcome):
        abort(404)
    return json.dumps({"index": index, "val": welcome[index]})

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET', 'POST'])
def api_request(path=None):
    resp = {
            "method": request.method,
            "url": request.url,
            "path": path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
            resp["POST"] = {
                    "data" : request.get_json(),
                    }
    return jsonify(resp)

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

# Create - Post - Change all the sentence
@app.route('/api/welcome/', methods=['POST'])
def create_welcome():
    global welcome
    data = request.get_json()
    if not data or 'sentence' not in data:
        return jsonify({"error": "Champ 'sentence' manquant"}), 400
    welcome = data['sentence']
    return '', 204

# Retreive - Get - Return the sentence
@app.route('/api/welcome/', methods=['GET'])
def return_welcome():
    return jsonify({"sentence" : welcome})

# Retreive - Get - Return the letter at x
@app.route('/api/welcome/<int:x>', methods=['GET'])
def retreive_letter(x):
    if x < 0 or x >= len(welcome):
        abort(404)
    return jsonify({"position": x, "letter": welcome[x]})

# Update - Put - Insert a new word at the position x
@app.route('/api/welcome/<int:x>', methods=['PUT'])
def update_word(x):
    global welcome
    data = request.get_json()
    if not data or 'word' not in data:
        return jsonify({"error": "Champ 'word' requis"}), 400
    word = data['word']
    welcome_phrase = welcome[:x] + word + welcome[x:]
    return '', 204

# Update - Patch - Change the  letter at the position x
@app.route('/api/welcome/<int:x>', methods=['PATCH'])
def update_letter(x):
    global welcome
    data = request.get_json()
    if not data or 'letter' not in data or len(data['letter']) != 1:
        return jsonify({"error": "Une seule lettre requise"}), 400
    if x < 0 or x >= len(welcome):
        abort(404)
    lst = list(welcome)
    lst[x] = data['letter']
    welcome_phrase = ''.join(lst)
    return '', 204

# Delete - Delete - Delete the letter at the position x
@app.route('/api/welcome/<int:x>', methods=['DELETE'])
def delete_letter(x):
    global welcome
    if x < 0 or x >= len(welcome):
        abort(404)
    welcome = welcome[:x] + welcome[x+1:]
    return '', 204
# Delete - Delete - Delete all the sentence
@app.route('/api/welcome/', methods=['DELETE'])
def delete_welcome():
    global welcome
    welcome = ''
    return '', 204

# Create - Post - retreive new temperature
@app.route('/api/temp/', methods=['POST'])
def ret_new_temp():
    global temperatures
    if ser is None or not ser.is_open:
        return jsonify({"error": "Port série non disponible"}), 500
    command = "GET_T\r\n"
    ser.write(command.encode())
    time.sleep(0.1)
    response = ser.readline().decode().strip()
    if response.startswith("T="):
        temp_str = response[2:response.index("H")]  # Enlève "T=" et après le H
        temp = int(temp_str, 16)
        temperatures.append(temp)
        return '', 204
    else:
        return jsonify({"error" : "pas de réponse de la STM"})

# Create - Post - retreive new pression
@app.route('/api/pression/', methods=['POST'])
def ret_new_pres():
    global pressions
    if ser is None or not ser.is_open:
        return jsonify({"error": "Port série non disponible"}), 500
    command = "GET_T\r\n"
    ser.write(command.encode())
    time.sleep(0.1)
    response = ser.readline().decode().strip()
    if response.startswith("T="):
        pres_str = response[2:response.index("H")]  # Enlève "P=" et après le H
        pres = int(pres_str, 16)
        pressions.append(pres)
        return '', 204
    else:
        return jsonify({"error" : "pas de réponse de la STM"})


# Retreive - Get - return all previous temperatures
@app.route('/api/temp/', methods=['GET'])
def get_all_temp():
    return jsonify({"temperatures" : temperatures})

# Retreive - Get - return temperature @ x
@app.route('/api/temp/<int:x>', methods=['GET'])
def get_temp(x):
    if x<0 or x>len(temperatures):
        abort(404)
    return jsonify({"position" : x ,"temperature" : temperatures[x]})

# Retreive - Get - return all previous pressions
@app.route('/api/pression/', methods=['GET'])
def get_all_pression():
    return jsonify({"pressions" : pressions})

# Retreive - Get - return pression @ x
@app.route('/api/pression/<int:x>', methods=['GET'])
def get_pression(x):
    if x<0 or x>len(pressions):
        abort(404)
    return jsonify({"position" : x, "pression" : pressions[x]})

# Retreive - Get - return scale (K)
@app.route('/api/scale/', methods=['GET'])
def get_scale():
    return jsonify({"K = " : K})

# Retreive - Get - return angle (temp*K)
@app.route('/api/angle/', methods=['GET'])
def get_angle():
    if not temperatures:
        abort(404)
    temp = temperatures[-1]
    angle = temp * K
    return jsonify({"angle" : angle})

# Update - Post - change k for new value
@app.route('/api/scale/<float:x>', methods=['POST'])
def change_scale(x):
    global K
    k = x
    return '', 204

# Delete - Delete - delete temp @ x
@app.route('/api/temp/<int:x>', methods=['DELETE'])
def del_temp(x):
    if x<0 or x>len(temperatures):
        abort(404)
    del temperatures[x]
    return '', 204

# Delete - Delete - delete press @ x
@app.route('/api/pression/<int:x>', methods=['DELETE'])
def del_pres(x):
    if x<0 or x>len(pressions):
        abort(404)
    del pressions[x]
    return '', 204