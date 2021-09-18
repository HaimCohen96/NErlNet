#include "include/nifpp.h"
#include <vector>
#include <deque>
#include "../cppSANN/src/Models/include/Model.h"
#include "include/bridgeController.h"

#include <iostream>
using std::cerr;
using std::endl;
#include <fstream>
using std::ofstream;
#include <cstdlib> // for exit function
#include <string>


#include <chrono>
using namespace std::chrono;

#define DEBUG_TRAIN_NIF 0
#define DEBUG_PREDICT_NIF 0
#define DEBUG_CREATE_NIF 0

// Codes for the module state
enum ModuleMode {CREATE = 0, TRAIN = 1, PREDICT = 2};

// ----- Structs ------

// Create model parameters struct
// Optimizer (optional) - default ADAM
// Learning_rate - number (0-1). Usually 1/number_of_samples
// Train_set_size - percentage number. Usually 70%-80%. Represents the portion of the data that we train
// Activation_list (optional) - list
// layers_sizes - list in erlang. represents the layers sizes of the neural network
struct CreateModelParam {

    int optimizer;
    unsigned long modelId;
    double learning_rate;
    std::vector<int> activation_list;
    std::vector<uint32_t> layers_sizes;
};

// Train mode parameters struct
// rows, col - Represents the rows and columns of the data matrix
// labels - Represents the label portion of the label matrix from the data_label matrix
// mid - model id number
// data_Label_mat - list in erlang. Represents tha data and the label matrix (last columns) together
// tid - unique thread identification
// pid - unique erlang process identification
struct TrainParam {

    int rows, col, labels;
    unsigned long mid;
    std::vector<double> data_label_mat;
    ErlNifTid tid;
    ErlNifPid pid;
};

// Predict mode parameters struct
// rows, col - Represents the rows and columns of the data matrix
// mid - model id number
// data_mat - The data matrix without the labels
// tid - unique thread identification
// pid - unique erlang process identification
struct PredictParam {

    int rows, cols;
    unsigned long mid;
    std::vector<double> data_mat;
    ErlNifTid tid;
    ErlNifPid pid;
};

//----------------------------------------------------------------------------------------------------------------------
// Delete model by mid from the map
static ERL_NIF_TERM cppBridgeControllerDeleteModel_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int mid;

    // Get mid (int) from erlang term
    if (!enif_get_int(env, argv[0], &mid)) {
        return enif_make_badarg(env);
    }

    cppBridgeController *s = s->GetInstance();
    s->deleteModel(mid);

    return enif_make_int(env, mid);
}
//----------------------------------------------------------------------------------------------------------------------

// ----- Weights functions -----

// Get weights
static ERL_NIF_TERM get_weights_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    std::vector<double> weights_vec_sizes, bias_vec_sizes, bias_list_vec, weights_list_vec;
    std::vector<std::shared_ptr<ANN::Weights>> vec_of_weights_ptr;
    MatrixXd* weights_mat;
    VectorXd weights_Vec;
    VectorXd* bias;
    ERL_NIF_TERM ret_tuple;
    int ModelId, bias_list_vec_index;
    
    // Get the model id
    nifpp::get_throws(env, argv[0], ModelId); // TODO: Add try catch 

    // Get the singleton instance
    cppBridgeController *s = s->GetInstance();

    // Get the model from the singleton
    std::shared_ptr<SANN::Model> modelPtr = s-> getModelPtr(ModelId);

    // Get the weights_list ptr
    modelPtr->get_weights(vec_of_weights_ptr);

    // Go over all the weights and biases 
    for (std::vector<std::shared_ptr<ANN::Weights>>::iterator it = vec_of_weights_ptr.begin() ; it != vec_of_weights_ptr.end() ; it++)
    {
        // Go over the weights matrixXD and put it to a c++ vector
        weights_mat = (*it)->get_weights_mat_ptr();

        // Add weights matrix rows number and columns number to the weights_vec_sizes
        weights_vec_sizes.push_back(weights_mat->rows());
        weights_vec_sizes.push_back(weights_mat->cols());

        // Create the result vector from the result matrix TODO: Native in Eigen optionally
        for (int r = 0; r < weights_mat->rows(); r++){
            for (int c = 0; c < weights_mat->cols(); c++){
                weights_list_vec.push_back(weights_mat->coeff(r,c));
            }
        }

        // Go over the bias vectorXD and put it to a c++ vector
        bias=(*it)->get_bias_ptr();

        // Add the bias size to the vector of bias sizes
        bias_vec_sizes.push_back(bias->size());

        for (bias_list_vec_index = 0; bias_list_vec_index< bias->size(); bias_list_vec_index++){
            bias_list_vec.push_back(bias->coeff(bias_list_vec_index));
        }
    }


#if DEBUG_TRAIN_NIF
    ofstream outdata; // outdata is to send the weights to a file
    outdata.open("NerlNifCppOut.txt"); // opens the file
    if( !outdata ) { // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }

    outdata << "Start weights_vec_sizes" << endl;
    for (const auto &e : weights_vec_sizes) outdata << e << "\n"; // Export bias_vec_sizes
    outdata << "Start wheigts: " << endl;
    for (const auto &e : weights_list_vec_string) outdata << e << "\n"; // Export weights_list_vec
    outdata << "Start bias_vec_sizes" << endl;
    for (const auto &e : bias_vec_sizes) outdata << e << "\n"; // Export bias_vec_sizes
    outdata << "Start biases" << endl;
    for (const auto &e : bias_list_vec) outdata << e << "\n"; // Export bias
    outdata << "Start ModelId" << endl;
    outdata << ModelId << "\n";
    outdata.close();
#endif

    // Convert the weights_list_vec to nif term
    nifpp::TERM ret_weights_list = nifpp::make(env, weights_list_vec);

    // Convert the bias_list_vec to nif term
    nifpp::TERM ret_bias_list_vec = nifpp::make(env, bias_list_vec);

    // Convert the weights_vec_sizes to a nif term
    nifpp::TERM ret_weights_vec_sizes_list = nifpp::make(env, weights_vec_sizes);

    // Convert the bias_vec_sizes to a nif term
    nifpp::TERM ret_bias_vec_sizes_list = nifpp::make(env, bias_vec_sizes);

    // Return tuple: All weights matrixes combined to a vector, All biases combined to a vector, Sizes of each weight matrixes/biases as lists
    ret_tuple = enif_make_tuple(env, 4, ret_weights_list, ret_bias_list_vec, ret_bias_vec_sizes_list, ret_weights_vec_sizes_list);

    // Return weights_list and bias_list and the size of them
    return ret_tuple;
}

// Set weights
static ERL_NIF_TERM set_weights_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    std::vector<double> weights_list_double, bias_list_double;
    std::vector<int> weights_vec_sizes_int, bias_vec_sizes_int;
    std::vector<std::shared_ptr<ANN::Weights>> vec_of_weights_ptr;
    VectorXd bias;
    int ModelId, sizes_vec_index = 0, bias_list_index = 0, weight_index = 0, weight_index_sum = 0, weight_index_place = 0;

     // Get the singleton instance
    cppBridgeController *s = s->GetInstance();

    // std::cout<<"SET IN THE NIF!"<<std::endl;

    try {
        // Get a list of weightes, bias list, list of weights sizes, list of biases sizes and modelId
        nifpp::get_throws(env, argv[0], weights_list_double);
        nifpp::get_throws(env, argv[1], bias_list_double);
        nifpp::get_throws(env, argv[2], bias_vec_sizes_int);
        nifpp::get_throws(env, argv[3], weights_vec_sizes_int);
        nifpp::get_throws(env, argv[4], ModelId);

        // Get the model from the singleton
        std::shared_ptr<SANN::Model> modelPtr = s-> getModelPtr(ModelId);

        // Get the weights_list ptr
        modelPtr->get_weights(vec_of_weights_ptr);

        // Go over all the weights and biases 
        for (std::vector<std::shared_ptr<ANN::Weights>>::iterator it = vec_of_weights_ptr.begin() ; it != vec_of_weights_ptr.end() ; it++)
        {
            //*weights_Mat = Map<MatrixXd,0, Stride<Dynamic,Dynamic>>(trainPtr->data_label_mat.data(), trainPtr->rows, trainPtr->col,Stride<Dynamic,Dynamic>(1, FeaturesAndLabels));
            // Convert the vector to a matrix. (TODO: Native to Eigen)
            // Create the matrix
            MatrixXd weights_Mat(weights_vec_sizes_int[weight_index],weights_vec_sizes_int[weight_index+1]);

            for (int r = 0; r < weights_vec_sizes_int[weight_index]; r++){ // Go over the rows
                for (int c = 0; c < weights_vec_sizes_int[weight_index+1]; c++){ // Go over the columns
                    weight_index_place = r*weights_vec_sizes_int[weight_index+1] + c + weight_index_sum;
                    weights_Mat(r,c) = weights_list_double[weight_index_place];
                }
            }

            weight_index_sum += weight_index_place + 1;
            weight_index += 2;

            // Set weights to the module
            (*it)->set_weights(weights_Mat);

            // Convert the vector to a VectorXd. Native to Eigen
            bias = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(&bias_list_double[bias_list_index], bias_vec_sizes_int[sizes_vec_index]);

            // Set biases to the module
            (*it)->set_bias(bias);

            // Next indexes place in the vector of sizes
            bias_list_index = bias_list_index + bias_vec_sizes_int[sizes_vec_index];
            sizes_vec_index++;
        }
 #if DEBUG_TRAIN_NIF
    ofstream outdata; // outdata is to send the weights to a file
    outdata.open("NerlNifCppOutSet.txt"); // opens the file
    if( !outdata ) { // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }
    outdata << "bias_list_double:" << endl;
    for (const auto &e : bias_list_double) outdata << e << "\n"; // Export bias
    outdata << "weights_list_double:" << endl;
    for (const auto &e : weights_list_double) outdata << e << "\n"; // Export weights_list_vec
    outdata << "weights_vec_sizes_int:" << endl;
    for (const auto &e : weights_vec_sizes_int) outdata << e << "\n"; // Export bias_vec_sizes
    outdata << "bias_vec_sizes_int:" << endl;
    for (const auto &e : bias_vec_sizes_int) outdata << e << "\n"; // Export bias_vec_sizes
    outdata << "ModelId:" << endl;
    outdata << ModelId << "\n";

    outdata.close();

#endif   
       
    }
    catch(nifpp::badarg){
        return enif_make_badarg(env);
    }

    // TODO: Optionally return something else 
    return enif_make_int(env, 0);
}

// Average weights
static ERL_NIF_TERM average_weights_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    std::vector<double> vec_of_weightsAndBiases_vec_double, sum_of_weights;
    int NumOfWeightsAndBiases, index = 0;
    
    try {
        // Get the list of weightes, bias list and size
        nifpp::get_throws(env, argv[0], vec_of_weightsAndBiases_vec_double);
        nifpp::get_throws(env, argv[1], NumOfWeightsAndBiases);

        // The size of each weight and bias vector
        int OneWheightAndBiasesSize = vec_of_weightsAndBiases_vec_double.size()/NumOfWeightsAndBiases;
        sum_of_weights.resize(OneWheightAndBiasesSize,0);

        // Sum all the biases and weights
        for(int vecIndex = 0; vecIndex < OneWheightAndBiasesSize; vecIndex++){
            for(int i = index; i < static_cast<int>(vec_of_weightsAndBiases_vec_double.size()); i+= OneWheightAndBiasesSize) {
                sum_of_weights[vecIndex] += vec_of_weightsAndBiases_vec_double[i];
            }
            index++;
        }

        // Divide the sum of weights and biases vector by NumOfWeightsAndBiases to get the average vector
        for(int i = 0; i < static_cast<int>(sum_of_weights.size()); i++) {
            sum_of_weights[i] = sum_of_weights[i] / NumOfWeightsAndBiases;
        }

        // Convert the vec_of_weights_vec to a nif term
        nifpp::TERM ret_average_weightsBias_list = nifpp::make(env, sum_of_weights);

        // Return weights_list and bias_list
        return ret_average_weightsBias_list;
    }
    catch(nifpp::badarg){
        return enif_make_badarg(env);
    }

    return enif_make_int(env, 0);
}

// ----- Threaded functions -----

// Predict function - runs on a separate thread (thread_create)
static void* predictFun(void *arg){

    PredictParam* predictPtr = (PredictParam*)arg;
    ErlNifEnv *env = enif_alloc_env();
    MatrixXd resultsMat;
    ERL_NIF_TERM pred_res_and_time;

    // Get the singleton instance
    cppBridgeController *s = s->GetInstance();

    // Get the model from the singleton
    std::shared_ptr<SANN::Model> modelPtr = s-> getModelPtr(predictPtr->mid);

    // Get the data(predict) matrix from the data_label_mat vector and initialize the data matrix.
    MatrixXd data_matrix(predictPtr->rows, predictPtr->cols);

    // Create the data matrix from a vector. Native in Eigen.
    data_matrix = Map<MatrixXd,0, Stride<Dynamic,Dynamic>>(predictPtr->data_mat.data(), predictPtr->rows, predictPtr->cols,Stride<Dynamic,Dynamic>(1, predictPtr->cols));

#if DEBUG_PREDICT_NIF
    std::cout << "Start predicting (inside the thread)." << '\n';
#endif

    // Start timer for the prediction in cppSANN
    auto start = high_resolution_clock::now();

    // Predict model with received parameters
    modelPtr->predict(data_matrix, resultsMat);


     ofstream outdata; // outdata is to send the weights to a file
    outdata.open("NerlNifCppOutPredictData.txt", std::ios_base::app); // opens the file
    if( !outdata ) { // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }
    for (int r = 0; r < predictPtr->rows; r++){
        // Create the data matrix from a vector
        for (int c = 0; c < predictPtr->cols; c++){
            outdata<< data_matrix(r,c) << " ";
        }
        for(int l = 0; l < 1; l++){
            outdata<< resultsMat(r,l) << " ";
        }
        outdata << "\n";
    }

    outdata.close();


    // Stop the timer and calculate the time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);


    // Create the result vector from the result matrix TODO: Native in Eigen optionally
    std::vector<double> results_vec;
    results_vec.resize(resultsMat.rows()*resultsMat.cols());

    for (int r = 0; r < resultsMat.rows(); r++){
        for (int c = 0; c < resultsMat.cols(); c++){
            results_vec[r*resultsMat.cols() + c] = resultsMat(r,c);
        }
    }

#if DEBUG_PREDICT_NIF
    // Print the result matrix
    std::cout<<"results inside the nif predict thread: \n"<<resultsMat<<std::endl;
#endif

    // Convert the result vector to nif term
    nifpp::TERM retResults = nifpp::make(env, results_vec);

    // Convert the loss value and the train duration value to a nif term
    pred_res_and_time = enif_make_tuple(env, 2, retResults,enif_make_double(env, duration.count()));

    // Sends a message to a process. Parameters:
    // caller_env - The environment of the calling process or callback. Must be NULL only if calling from a custom thread not spawned by ERTS.
    // *to_pid - The pid of the receiving process. The pid is to refer to a process on the local node.
    // msg_env - The environment of the message term. Must be a process independent environment allocated with enif_alloc_env or NULL.
    // msg - The message term to send.
    if(enif_send(NULL,&(predictPtr->pid), env,pred_res_and_time))
    {
    #if DEBUG_PREDICT_NIF
        printf("enif_send succeed\n");
    #endif
    }
    else
        printf("enif_send failed\n");

    // Frees all terms in an environment and clears it for reuse.
    enif_clear_env(env);
    delete predictPtr; // TODO: Check for memory leaks

    #if DEBUG_PREDICT_NIF
    printf("Finish predict fun thread in the nif.\n");
    #endif

    return 0;
}

// Train function - runs on a separate thread (thread_create)
static void* trainFun(void *arg){

    TrainParam* trainPtr = (TrainParam*)arg;
    double loss_val;
    ErlNifEnv *env = enif_alloc_env();
    ERL_NIF_TERM loss_val_term;

    // Get the singleton instance
    cppBridgeController *s = s->GetInstance();

    // Get the model from the singleton
    std::shared_ptr<SANN::Model> modelPtr = s-> getModelPtr(trainPtr->mid);

    // Get the data matrix from the data_label_mat vector and initialize the data matrix.
    MatrixXd data_mat(trainPtr->rows, trainPtr->col);

    // Get the label matrix from the data_label_mat vector and initialize the label matrix
    MatrixXd label_mat(trainPtr->rows, trainPtr->labels);

    // Convert the vector to a matrix. Native to Eigen
    int FeaturesAndLabels = trainPtr->col+trainPtr->labels; // Number of columns in total
    data_mat = Map<MatrixXd,0, Stride<Dynamic,Dynamic>>(trainPtr->data_label_mat.data(), trainPtr->rows, trainPtr->col,Stride<Dynamic,Dynamic>(1, FeaturesAndLabels));
    label_mat = Map<MatrixXd,0, Stride<Dynamic,Dynamic>>(&trainPtr->data_label_mat[trainPtr->col], trainPtr->rows, trainPtr->labels,Stride<Dynamic,Dynamic>(1, FeaturesAndLabels));

    ofstream outdata; // outdata is to send the weights to a file
    outdata.open("NerlNifCppOutTrainedData.txt", std::ios_base::app); // opens the file
    if( !outdata ) { // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }
    for (int r = 0; r < trainPtr->rows; r++){
        // Create the data matrix from a vector
        for (int c = 0; c < trainPtr->col; c++){
            outdata<< data_mat(r,c) << " ";
        }
        for(int l = 0; l < trainPtr->labels; l++){
            outdata<< label_mat(r,l) << " ";
        }
        outdata << "\n";
    }

    outdata.close();
    
    #if DEBUG_TRAIN_NIF
    std::cout <<"data mat: "<<std::endl;
    std::cout <<data_mat<<std::endl;
    std::cout <<"label mat: "<<std::endl;
    std::cout <<label_mat<<std::endl;

    printf("Finish to go over the list.\n");

    // Print the Data (train features) matrix
    std::cout<<"Data: "<<std::endl;
     for (int r = 0; r < trainPtr->rows; r++){
            // Create the data matrix from a vector
            for (int c = 0; c < trainPtr->col; c++){
                std::cout<< data_mat(r,c) << " ";
            }
            std::cout<<"\n"<<std::endl;
     }

     // Print the label (train label) matrix
     std::cout<<"Label: "<<std::endl;
     for (int r = 0; r < trainPtr->rows; r++){
             // Create the label matrix from a vector
             for(int l = 0; l < trainPtr->labels; l++){
                 std::cout<< label_mat(r,l) << " ";
             }
             std::cout<<"\n"<<std::endl;
     }

    printf("Start to train the model.\n");
    #endif

     // Start the timer for the training in cppSANN
    auto start = high_resolution_clock::now();

    // Train the model with recieved parameters and get loss value
    loss_val = modelPtr->train(data_mat,label_mat);

    // Stop the timer and calculate the time took for training
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

#if DEBUG_TRAIN_NIF
    // To get the value of duration use the count()
    // member function on the duration object
    std::cout << "Train time inside the NIF (micro seconds): " << duration.count() << std::endl;

    printf("Finish train the model, loss fun inside the nif thread: \n");
    std::cout<< loss_val << "\n";
#endif

    // Check if the loss value is in range and not -NaN
    if( loss_val > 1.7976931348623157e+308 || loss_val < -1.7976931348623157e+308 || loss_val != loss_val)
    {
        printf("Loss value not in range \n");

        // Convert the loss value to a nif term
        loss_val_term = enif_make_atom(env, "nan");
        #if DEBUG_PREDICT_NIF
                std::cout << "loss_val_term: \n" << loss_val_term << "\n";
        #endif
    }
    else{
        // Convert the loss value and the train duration value to a nif term
        loss_val_term = enif_make_tuple(env, 2, enif_make_double(env, loss_val),enif_make_double(env, duration.count()));
        #if DEBUG_PREDICT_NIF
                std::cout << "loss_val_term: \n" << loss_val_term << "\n";
        #endif
    }

    // Send to erlang process the loss value and time
    if(enif_send(NULL,&(trainPtr->pid), env,loss_val_term)){
    #if DEBUG_PREDICT_NIF
            printf("enif_send succeed\n");
    #endif
    }
    else
        printf("enif_send failed\n");

    // Frees all terms in an environment and clears it for reuse.
    enif_clear_env(env);

    delete trainPtr;
#if DEBUG_PREDICT_NIF
    printf("Finish train fun thread in the nif.\n");
#endif

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------

// Train, Predict and Create module nif - Main nif (The NIF starts executing here)
// All terms of type ERL_NIF_TERM belong to an environment of type ErlNifEnv.
// The lifetime of a term is controlled by the lifetime of its environment object.
// All API functions that read or write terms has the environment that the term belongs to as the first function argument.
static ERL_NIF_TERM train_predict_create_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    #if DEBUG_PREDICT_NIF
        std::cout << "Start the NIF." << '\n';
    #endif

    // mode = 0 - model creation, 1 - train, 2 - predict
    int mode;

    // Get mode (int) from erlang term
    if (!enif_get_int(env, argv[0], &mode)) {
        return enif_make_badarg(env);
    }

    // Create model - 0
    if(mode == CREATE){

        // Struct that stores the model create parameters
        CreateModelParam modelParamPtr = CreateModelParam();

        try{
            // Convert the erlang nif types to C++ types
            // get_throws is a template wrapper for enif_get_XXX(). Provides the option to use c++ types.
            // It will throw nifpp::badarg upon failure unlike nifpp::get() that will return true on success, false on failure.
            nifpp::get_throws(env, argv[1], modelParamPtr.layers_sizes);
            nifpp::get_throws(env, argv[2], modelParamPtr.learning_rate);
            nifpp::get_throws(env, argv[3], modelParamPtr.modelId);
            nifpp::get_throws(env, argv[4], modelParamPtr.activation_list);
            nifpp::get_throws(env, argv[5], modelParamPtr.optimizer);

            // Create the activation list vector from the received activation list
            std::vector<act_t> act_types_vec;
            for(int i = 0; i < (int)(modelParamPtr.activation_list.size()); i++){
                act_types_vec.push_back(static_cast<act_t>(modelParamPtr.activation_list[i]));
            }

            Optimizers::opt_t optimizer = (Optimizers::opt_t)modelParamPtr.optimizer;

#if DEBUG_CREATE_NIF
            std::cout << "Optimizers::opt_t optimizer: " << optimizer << '\n';
            std::cout << "act_types_vec: " << act_types_vec << '\n';
            std::cout << "modelParamPtr.layers_sizes: " << modelParamPtr.layers_sizes << '\n';
            std::cout << "modelParamPtr.learning_rate: " << modelParamPtr.learning_rate << '\n';
             std::cout << "Start creating the module." << '\n';
#endif

            // Create the model with the received parameters. TODO: Think if unique pointer is a better option
            SANN::Model *model = new SANN::Model(modelParamPtr.layers_sizes,modelParamPtr.learning_rate);
            std::shared_ptr<SANN::Model> modelPtr(model);

            // Set activations list
            modelPtr->set_activations(act_types_vec);

            // Set the optimizer
            modelPtr->set_optimizer(optimizer);

            // Create the singleton instance
            cppBridgeController *s = s->GetInstance();

            // Put the model record to the map with modelId
            s->setData(modelPtr, modelParamPtr.modelId);

#if DEBUG_PREDICT_NIF
            printf("Finish creating the module in the nif.\n");
#endif

            return enif_make_int(env, 0); // TODO: change the return value if needed
        }
        catch(nifpp::badarg){
            return enif_make_badarg(env);
        }
    }
    // Train mode - 1
    else if(mode == TRAIN){

        // Struct that stores the model train parameters
        TrainParam* trainPtr = new TrainParam(); //TODO: Delete It (memmory) and in the future think about changing it to shared/unique pointers/regular stack struct

        try{
            // Convert the erlang nif types to C++ types
            nifpp::get_throws(env, argv[1], trainPtr->rows);
            nifpp::get_throws(env, argv[2], trainPtr->col);
            nifpp::get_throws(env, argv[3], trainPtr->labels);
            nifpp::get_throws(env, argv[4], trainPtr->data_label_mat);
            nifpp::get_throws(env, argv[5], trainPtr->mid); // model id

            // Get the pid of the calling process
            ErlNifPid pid;
            enif_self(env, &pid);
            trainPtr->pid = pid;

            // Start a thread with trainFun function to train the model in a separate thread
            // int enif_thread_create(char *name, ErlNifTid *tid, void * (*func)(void *), void *args, ErlNifThreadOpts *opts)
            // name -A string identifying the created thread. It is used to identify the thread in planned future debug functionality.
            // tid - A pointer to a thread identifier variable.
            // func - A pointer to a function to execute in the created thread.
            // arg - A pointer to argument to the func function.
            // opts - A pointer to thread options to use or NULL.
            // Returns 0 on success, otherwise an errno value is returned to indicate the error.
            // The newly created thread begins executing in the function pointed to by func, and func is passed arg as argument.
            // When erl_drv_thread_create returns, the thread identifier of the newly created thread is available in *tid.
            // opts can be either a NULL pointer, or a pointer to an ErlDrvThreadOpts structure.
            // If opts is a NULL pointer (0), default options are used, otherwise the passed options are used.
            int res = enif_thread_create((char*)"trainModule", &(trainPtr->tid), trainFun, trainPtr, 0);

        #if DEBUG_PREDICT_NIF
                    printf("Finish train module nif (Not the thread itself).\n");
        #endif

            return enif_make_int(env, res); // 0 - means the thread succeeded to run
        }
        catch(nifpp::badarg){
           return enif_make_badarg(env);
        }
    }
    // Predict mode - 2
    else if (mode == PREDICT){

        // Struct that stores the model train parameters
        PredictParam* predictPtr = new PredictParam(); //TODO: Delete It (memmory) and in the future think about changing it to shared/unique pointers/regular stack struct

        // Convert the erlang nif types to C++ types
        nifpp::get_throws(env, argv[1], predictPtr->data_mat);
        nifpp::get_throws(env, argv[2], predictPtr->rows);
        nifpp::get_throws(env, argv[3], predictPtr->cols);
        nifpp::get_throws(env, argv[4], predictPtr->mid); // model id

        // Get the pid of the calling process
        ErlNifPid pid;
        enif_self(env, &pid);
        predictPtr->pid = pid;

        // Create the thread to run predict
        int res = enif_thread_create((char*)"predictModule", &(predictPtr->tid), predictFun, predictPtr, 0);

#if DEBUG_PREDICT_NIF
        printf("Finish predict nif (Not the thread itself).\n");
#endif
        return enif_make_int(env, res); // 0 - means the thread succeeded to run
    }

    // This shouldn't happen in a regular situation
    std::cout << "Incorrect mode entered: " << mode << ". Please run with the correct mode: 0 - Create, 1 - Train or 2 - Predict." << '\n';
    return enif_make_int(env, mode);
}
//----------------------------------------------------------------------------------------------------------------------
// ---- NIF Settings ----

// Describes a NIF by its name in erlang, arity, implementation in c/c++ (ERL_NIF_TERM) and dirty nif flag.
static ErlNifFunc nif_funcs[] = {
    {"train_predict_create", 6, train_predict_create_nif,ERL_NIF_DIRTY_JOB_CPU_BOUND}, // For train
    {"train_predict_create", 5, train_predict_create_nif,ERL_NIF_DIRTY_JOB_CPU_BOUND}, // For predict
    {"create_module", 6, train_predict_create_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND}, // For module create. TODO: Think about using it in a dirty scheduler
    {"cppBridgeControllerDeleteModel", 1, cppBridgeControllerDeleteModel_nif}, // Delete model by mid
    {"average_weights", 2, average_weights_nif}, // Average weights
    {"get_weights", 1, get_weights_nif}, // Get weights
    {"set_weights", 5, set_weights_nif} // Set weights
};

// TODO: Think about using this feature in the future
// load_info is the second argument to erlang:load_nif/2.
// *priv_data can be set to point to some private data if the library needs to keep a state between NIF calls.
// enif_priv_data returns this pointer. *priv_data is initialized to NULL when load is called.
// The library fails to load if load returns anything other than 0. load can be NULL if initialization is not needed.
static int load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    //nifpp::register_resource<GetcppBridgeController>(env, nullptr, "GetcppBridgeController");
    //nifpp::register_resource<cppBridgeController>(env, nullptr, "cppBridgeController");
   // nifpp::register_resource<SANN::Model>(env, nullptr, "cppBridgeController");
    return 0;
}

// This is the magic macro to initialize a NIF library. It is to be evaluated in global file scope.
// ERL_NIF_INIT(MODULE, ErlNifFunc funcs[], load, NULL, upgrade, unload)
// MODULE -  The first argument must be the name of the Erlang module as a C-identifier. It will be stringified by the macro.
// ErlNifFunc - The second argument is the array of ErlNifFunc structures containing name, arity, and function pointer of each NIF.
// load -  is called when the NIF library is loaded and no previously loaded library exists for this module.
// NULL - The fourth argument NULL is ignored. It was earlier used for the deprecated reload callback which is no longer supported since OTP 20.
// The remaining arguments are pointers to callback functions that can be used to initialize the library.
// They are not used in this simple example, hence they are all set to NULL.
ERL_NIF_INIT(erlModule, nif_funcs, load, NULL, NULL, NULL)