from flask import Flask, render_template, request, jsonify, abort
import json
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!\n'

welcome = "Welcome to 3ESE API!"

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
@app.route('/api/welcome/', methods=['GET'])
def retreive_letter(x):
    if x < 0 or x >= len(welcome):
        abort(404)
    return jsonify({"position": x, "letter": welcome[x]})

# Update - Put - Insert a new word at the position x
@app.route('/api/welcome/', methods=['PUT'])
def update_word(x):
    global welcome
    data = request.get_json()
    if not data or 'word' not in data:
        return jsonify({"error": "Champ 'word' requis"}), 400
    word = data['word']
    welcome = welcome[:x] + word + welcome[x:]
    return '', 204

# Update - Patch - Change the  letter at the position x
@app.route('/api/welcome/', methods=['PATCH'])
def update_letter(x):
    global welcome
    data = request.get_json()
    if not data or 'letter' not in data or len(data['letter']) != 1:
        return jsonify({"error": "Une seule lettre requise"}), 400
    if x < 0 or x >= len(welcome):
        abort(404)
    lst = list(welcome)
    lst[x] = data['letter']
    welcome = ''.join(lst)
    return '', 204

# Delete - Delete - Delete the letter at the position x
@app.route('/api/welcome/', methods=['DELETE'])
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