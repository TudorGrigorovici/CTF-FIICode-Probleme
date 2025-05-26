from flask import *
import base64
import os

app=Flask(__name__)
id=3

@app.get('/')
def getRoutes():
    return Response(status=200,
                    content_type='application/json',
                    response=json.dumps({
                        "v2":'/api/v2/'
                    })
                    )

@app.get('/api/v1')
def helpV1():
    return Response(
        status=200,
        content_type='application/json',
        response=json.dumps({
            "GET /api/v1/order": {
                "description": "Get a JSON order by base64-encoded file name.",
                "example": "/api/v1/order?file=order_1.json"
            },
            "HINT":str('Try Harder\n'*id)
        }, indent=2)
    )

@app.get('/api/v2')
def helpV2():
    return Response(
        status=200,
        content_type='application/json',
        response=json.dumps({
            "GET /api/v2/order": {
                "description": "Get a JSON order by base64-encoded ID.",
                "example": "/api/v2/order?id=MQ=="
            },
            "POST /api/v2/orders": {
                "description": "Submit a new order (auto ID assigned).",
                "example": {
                    "curl": "curl -X POST http://host:5000/api/v2/orders -H 'Content-Type: application/json' -d '{\"customer\":\"ionut\",\"items\":[\"chicken leg\"]}'"
                }
            },
            "POST /ap1/v2/complaint": {
                "description": "Submit a complaint.",
                "example": {
                    "curl": "curl -X POST http://host:5000/ap1/v2/complaint -H 'Content-Type: application/json' -d '{\"complaint\":\"this app sucks\"}'"
                }
            }
        }, indent=2)
    )

@app.get('/api/v1/order')
def getOrder():
    file = request.args.get('file')  # LFI vulnerable
    try:
        with open(f'/apps/api/orders/{file}', 'rb') as fd:
            data = fd.read()  # Just read raw content
            return Response(status=200,
                            content_type='text/plain',
                            response=data)
    except FileNotFoundError:
        return Response(status=404,
                        content_type='application/json',
                        response=json.dumps({"message": "File not found"}))
    except Exception as e:
        return Response(status=500,
                        content_type='application/json',
                        response=json.dumps({"error": f"{e}"}))


@app.get('/api/v2/order')
def getOrder2():
    id = base64.b64decode(request.args.get('id')).decode('utf-8')
    print(id)
    try:
        with open(f'/apps/api/orders/order_{id}.json','rb') as fd:
            data=json.load(fd)
            return Response(status=200,
                            content_type='application/json',
                            response=json.dumps(data)
                            )
    except FileNotFoundError:
        return Response(status=404,
                        content_type='application/json',
                        response=json.dumps({"message":"Order not found"})
                        )
    except Exception as e:
        return Response(status=500,
                        content_type='application/json',
                        response=json.dumps({"error":f'{e}'}))

@app.post('/api/v2/orders')
def postOrder():
    global id
    try:
        data = request.get_json()

        filepath = os.path.join('/apps/api/orders', f'order_{id}.json')
        with open(filepath, 'w') as fd:
            json.dump(data, fd)

        encoded_id = base64.b64encode(str(id).encode()).decode()
        response_data = {"message": "Order saved", "id": encoded_id}

        id += 1 

        return Response(
            status=201,
            content_type='application/json',
            response=json.dumps(response_data)
        )

    except Exception as e:
        return Response(
            status=500,
            content_type='application/json',
            response=json.dumps({"error": f'{e}'})
        )


@app.post('/ap1/v2/complaint')
def postComplaint():
    data = request.get_json()
    complaint = data.get('complaint', '')

    if '<script>' in complaint.lower():
        return Response(
            status=200,
            content_type='application/json',
            response=json.dumps({
                'message': 'Nice try, try harder... or should you...'
            })
        )

    with open('complaints.log', 'a') as log:
        log.write(complaint + '\n')

    return Response(
        status=200,
        content_type='application/json',
        response=json.dumps({
            'message': 'Stop bitching bro...'
        })
    )


if __name__=='__main__':
    app.run(host='0.0.0.0',port=5000,debug=True)